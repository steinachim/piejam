// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/persistence/access.h>

#include <piejam/entity_id_hash.h>
#include <piejam/runtime/actions/apply_app_config.h>
#include <piejam/runtime/actions/apply_session.h>
#include <piejam/runtime/fx/unavailable_ladspa.h>
#include <piejam/runtime/persistence/app_config.h>
#include <piejam/runtime/persistence/session.h>
#include <piejam/runtime/state.h>
#include <piejam/runtime/ui/thunk_action.h>

#include <spdlog/spdlog.h>

#include <boost/assert.hpp>

#include <algorithm>
#include <filesystem>
#include <fstream>

namespace piejam::runtime::persistence
{

void
load_app_config(std::filesystem::path const& file, dispatch_f const& dispatch)
{
    try
    {
        std::ifstream in(file);
        if (!in.is_open())
            throw std::runtime_error("could not open config file");

        actions::apply_app_config action;
        action.conf = persistence::load_app_config(in);
        dispatch(action);
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not load config file: {}", err.what());
    }
}

void
save_app_config(
        std::filesystem::path const& file,
        std::vector<std::string> const& enabled_midi_input_devices,
        state const& state)
{
    try
    {
        std::ofstream out(file);
        if (!out.is_open())
            throw std::runtime_error("could not open config file");

        persistence::app_config conf;

        conf.input_device_name =
                state.pcm_devices->inputs[state.input.index].name;
        conf.output_device_name =
                state.pcm_devices->outputs[state.output.index].name;
        conf.samplerate = state.samplerate;
        conf.period_size = state.period_size;

        auto const buses_to_bus_configs = [](auto const& chs,
                                             auto const& ch_ids,
                                             auto& configs) {
            configs.reserve(ch_ids.size());
            std::ranges::transform(
                    ch_ids,
                    std::back_inserter(configs),
                    [&chs](mixer::bus_id const& ch_id)
                            -> persistence::bus_config {
                        mixer::bus const* const bus = chs[ch_id];
                        return {bus->name, bus->type, bus->device_channels};
                    });
        };

        buses_to_bus_configs(
                state.mixer_state.buses,
                state.mixer_state.inputs.get(),
                conf.input_bus_config);

        buses_to_bus_configs(
                state.mixer_state.buses,
                state.mixer_state.outputs.get(),
                conf.output_bus_config);

        conf.enabled_midi_input_devices = enabled_midi_input_devices;

        persistence::save_app_config(out, conf);
    }
    catch (std::exception const& err)
    {
        spdlog::error("could not save config file: {}", err.what());
    }
}

static auto
export_parameter_values(fx::module const& fx_mod, parameter_maps const& params)
        -> std::vector<fx::parameter_value_assignment>
{
    std::vector<fx::parameter_value_assignment> result;

    for (auto&& [key, fx_param_id] : *fx_mod.parameters)
    {
        std::visit(
                [&](auto&& param) {
                    auto const* const value = params.get(param);
                    BOOST_ASSERT(value);
                    result.emplace_back(
                            fx::parameter_value_assignment{key, *value});
                },
                fx_param_id);
    }

    return result;
}

static auto
export_fx_midi_assignments(
        fx::module const& fx_mod,
        midi_assignments_map const& midi_assigns)
        -> std::vector<fx::parameter_midi_assignment>
{
    std::vector<fx::parameter_midi_assignment> result;

    for (auto&& [key, fx_param_id] : *fx_mod.parameters)
    {
        std::visit(
                [&](auto&& param) {
                    if (auto it = midi_assigns.find(param);
                        it != midi_assigns.end())
                    {
                        result.emplace_back(
                                fx::parameter_midi_assignment{key, it->second});
                    }
                },
                fx_param_id);
    }

    return result;
}

static auto
export_fx_plugin(
        state const& st,
        fx::module const& fx_mod,
        fx::internal const fx_type) -> session::fx_plugin
{
    BOOST_ASSERT(std::get<fx::internal>(fx_mod.fx_instance_id) == fx_type);

    session::internal_fx fx;
    fx.type = fx_type;
    fx.preset = export_parameter_values(fx_mod, st.params);
    fx.midi = export_fx_midi_assignments(fx_mod, st.midi_assignments);
    return fx;
}

static auto
export_fx_plugin(
        state const& st,
        fx::module const& fx_mod,
        fx::ladspa_instance_id const id) -> session::fx_plugin
{
    BOOST_ASSERT(std::get<fx::ladspa_instance_id>(fx_mod.fx_instance_id) == id);

    session::ladspa_plugin plug;
    auto const& pd = st.fx_ladspa_instances->at(id);
    plug.id = pd.id;
    plug.name = pd.name;
    plug.preset = export_parameter_values(fx_mod, st.params);
    plug.midi = export_fx_midi_assignments(fx_mod, st.midi_assignments);
    return plug;
}

static auto
export_fx_plugin(
        state const& st,
        fx::module const& fx_mod,
        fx::unavailable_ladspa_id const id) -> session::fx_plugin
{
    BOOST_ASSERT(
            std::get<fx::unavailable_ladspa_id>(fx_mod.fx_instance_id) == id);

    auto unavail = st.fx_unavailable_ladspa_plugins[id];
    BOOST_ASSERT(unavail);
    session::ladspa_plugin plug;
    plug.id = unavail->plugin_id;
    plug.name = fx_mod.name;
    plug.preset = unavail->parameter_values;
    plug.midi = unavail->midi_assigns;
    return plug;
}

static auto
export_fx_chain(state const& st, fx::chain_t const& fx_chain)
        -> persistence::session::fx_chain_t
{
    persistence::session::fx_chain_t result;

    for (auto const& fx_mod_id : fx_chain)
    {
        fx::module const* const fx_mod = st.fx_modules[fx_mod_id];
        result.emplace_back(std::visit(
                [&st, fx_mod](auto const& id) {
                    return export_fx_plugin(st, *fx_mod, id);
                },
                fx_mod->fx_instance_id));
    }

    return result;
}

static auto
export_mixer_midi(state const& st, mixer::bus const& bus)
        -> persistence::session::mixer_midi
{
    persistence::session::mixer_midi result;

    if (auto it = st.midi_assignments->find(bus.volume);
        it != st.midi_assignments->end())
        result.volume = it->second;

    if (auto it = st.midi_assignments->find(bus.pan_balance);
        it != st.midi_assignments->end())
        result.pan = it->second;

    if (auto it = st.midi_assignments->find(bus.mute);
        it != st.midi_assignments->end())
        result.mute = it->second;

    return result;
}

static auto
export_mixer_buses(state const& st, mixer::bus_list_t const& bus_ids)
{
    std::vector<persistence::session::mixer_bus> result;

    for (auto const& bus_id : bus_ids)
    {
        mixer::bus const* const bus = st.mixer_state.buses[bus_id];

        auto& mb_data = result.emplace_back();
        mb_data.fx_chain = export_fx_chain(st, *bus->fx_chain);
        mb_data.midi = export_mixer_midi(st, *bus);
    }

    return result;
}

void
load_session(std::filesystem::path const& file, dispatch_f const& dispatch)
{
    try
    {
        std::ifstream in(file);
        if (!in.is_open())
            throw std::runtime_error("could not open session file");

        dispatch(actions::apply_session(persistence::load_session(in)));
    }
    catch (std::exception const& err)
    {
        spdlog::error("load_session: {}", err.what());
    }
}

void
save_session(std::filesystem::path const& file, state const& st)
{
    try
    {
        std::ofstream out(file);
        if (!out.is_open())
            throw std::runtime_error("could not open session file");

        session ses;

        ses.inputs = export_mixer_buses(st, *st.mixer_state.inputs);
        ses.outputs = export_mixer_buses(st, *st.mixer_state.outputs);

        save_session(out, ses);
    }
    catch (std::exception const& err)
    {
        spdlog::error("save_session: {}", err.what());
    }
}

} // namespace piejam::runtime::persistence
