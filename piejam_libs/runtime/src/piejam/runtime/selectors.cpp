// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/runtime/selectors.h>

#include <piejam/runtime/fx/registry.h>
#include <piejam/runtime/solo_group.h>
#include <piejam/runtime/state.h>

#include <piejam/algorithm/transform_to_vector.h>
#include <piejam/audio/multichannel_buffer.h>
#include <piejam/functional/memo.h>
#include <piejam/npos.h>
#include <piejam/range/indices.h>
#include <piejam/reselect/selector.h>

#include <boost/hof/match.hpp>
#include <boost/hof/unpack.hpp>

namespace piejam::runtime::selectors
{

selector<box<sample_rate_choice>> const select_sample_rate([](state const& st) {
    static auto const get_sample_rate = memo(
            [](unique_box<audio::sound_card_hw_params> const& input_hw_params,
               unique_box<audio::sound_card_hw_params> const& output_hw_params,
               audio::sample_rate const current) {
                return box<sample_rate_choice>{
                        std::in_place,
                        runtime::sample_rates(
                                input_hw_params,
                                output_hw_params),
                        current};
            });

    return get_sample_rate(
            st.selected_io_sound_card.in.hw_params,
            st.selected_io_sound_card.out.hw_params,
            st.sample_rate);
});

selector<box<period_size_choice>> const select_period_size([](state const& st) {
    static auto const get_period_size = memo(
            [](unique_box<audio::sound_card_hw_params> const& input_hw_params,
               unique_box<audio::sound_card_hw_params> const& output_hw_params,
               audio::period_size const current) {
                return box<period_size_choice>{
                        std::in_place,
                        runtime::period_sizes(
                                input_hw_params,
                                output_hw_params),
                        current};
            });

    return get_period_size(
            st.selected_io_sound_card.in.hw_params,
            st.selected_io_sound_card.out.hw_params,
            st.period_size);
});

selector<box<period_count_choice>> const select_period_count([](state const&
                                                                        st) {
    static auto get_period_count = memo(
            [](unique_box<audio::sound_card_hw_params> const& input_hw_params,
               unique_box<audio::sound_card_hw_params> const& output_hw_params,
               audio::period_count const current) {
                return box<period_count_choice>{
                        std::in_place,
                        runtime::period_counts(
                                input_hw_params,
                                output_hw_params),
                        current};
            });

    return get_period_count(
            st.selected_io_sound_card.in.hw_params,
            st.selected_io_sound_card.out.hw_params,
            st.period_count);
});

selector<float> const select_buffer_latency([](state const& st) {
    return st.sample_rate.value() != 0
                   ? (st.period_size.value() * st.period_count.value() * 1000.f) /
                             st.sample_rate.as_float()
                   : 0.f;
});

static auto get_sound_card = memo(
        [](unique_box<std::vector<audio::sound_card_descriptor>> const& descs,
           std::size_t const index) {
            return box<sound_card_choice>{
                    std::in_place,
                    algorithm::transform_to_vector(
                            descs.get(),
                            &audio::sound_card_descriptor::name),
                    index};
        });

selector<box<sound_card_choice>> const
        select_input_sound_card([](state const& st) {
            return get_sound_card(
                    st.io_sound_cards.in,
                    st.selected_io_sound_card.in.index);
        });

selector<box<sound_card_choice>> const
        select_output_sound_card([](state const& st) {
            return get_sound_card(
                    st.io_sound_cards.out,
                    st.selected_io_sound_card.out.index);
        });

auto
make_num_device_channels_selector(io_direction const io_dir)
        -> selector<std::size_t>
{
    switch (io_dir)
    {
        case io_direction::input:
            return selector<std::size_t>([](state const& st) -> std::size_t {
                return st.selected_io_sound_card.in.hw_params->num_channels;
            });

        case io_direction::output:
            return selector<std::size_t>([](state const& st) -> std::size_t {
                return st.selected_io_sound_card.out.hw_params->num_channels;
            });
    }
}

auto
make_external_audio_device_ids_selector(io_direction const io_dir)
        -> selector<unique_box<external_audio::device_ids_t>>
{
    switch (io_dir)
    {
        case io_direction::input:
            return [](state const& st) {
                return st.external_audio_state.inputs;
            };

        case io_direction::output:
            return [](state const& st) {
                return st.external_audio_state.outputs;
            };
    }
}

template <auto GetMember>
static auto
make_external_audio_member_selector(
        external_audio::device_id const device_id,
        decltype(std::invoke(
                GetMember,
                std::declval<external_audio::device const&>())) default_value =
                {})
{
    auto get = memo([device_id, default_value = std::move(default_value)](
                            external_audio::devices_t const& devices) {
        external_audio::device const* const device = devices.find(device_id);
        return device ? std::invoke(GetMember, *device) : default_value;
    });

    return [get = std::move(get)](state const& st) {
        return get(st.external_audio_state.devices);
    };
}

auto
make_external_audio_device_name_selector(
        external_audio::device_id const device_id) -> selector<boxed_string>
{
    return make_external_audio_member_selector<&external_audio::device::name>(
            device_id);
}

auto
make_external_audio_device_bus_type_selector(
        external_audio::device_id const device_id) -> selector<audio::bus_type>
{
    return make_external_audio_member_selector<
            &external_audio::device::bus_type>(device_id);
}

template <audio::bus_channel BusChannel>
static auto
get_external_audio_device_bus_channel(external_audio::device const& device)
{
    return BusChannel == audio::bus_channel::right ? device.channels.right
                                                   : device.channels.left;
}

auto
make_external_audio_device_bus_channel_selector(
        external_audio::device_id const device_id,
        audio::bus_channel const bus_channel) -> selector<std::size_t>
{
    switch (bus_channel)
    {
        case audio::bus_channel::right:
            return make_external_audio_member_selector<
                    &get_external_audio_device_bus_channel<
                            audio::bus_channel::right>>(
                    device_id,
                    piejam::npos);

        default:
            return make_external_audio_member_selector<
                    &get_external_audio_device_bus_channel<
                            audio::bus_channel::left>>(device_id, piejam::npos);
    }
}

selector<unique_box<mixer::channel_ids_t>> const select_mixer_user_channels(
        [](state const& st) { return st.mixer_state.inputs; });

selector<mixer::channel_id> const select_mixer_main_channel(
        [](state const& st) { return st.mixer_state.main; });

template <auto GetMember>
static auto
make_mixer_channel_member_selector(
        mixer::channel_id const channel_id,
        decltype(std::invoke(GetMember, std::declval<mixer::channel const&>()))
                default_value = {})
{
    auto get = memo([channel_id, default_value = std::move(default_value)](
                            mixer::channels_t const& channels) {
        mixer::channel const* const channel = channels.find(channel_id);
        return channel ? std::invoke(GetMember, *channel) : default_value;
    });

    return [get = std::move(get)](state const& st) {
        return get(st.mixer_state.channels);
    };
}

auto
make_mixer_channel_bus_type_selector(mixer::channel_id const channel_id)
        -> selector<audio::bus_type>
{
    return make_mixer_channel_member_selector<&mixer::channel::bus_type>(
            channel_id);
}

auto
make_mixer_channel_volume_parameter_selector(mixer::channel_id const channel_id)
        -> selector<float_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::volume>(
            channel_id);
}

auto
make_mixer_channel_pan_balance_parameter_selector(
        mixer::channel_id const channel_id) -> selector<float_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::pan_balance>(
            channel_id);
}

auto
make_mixer_channel_record_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::record>(
            channel_id);
}

auto
make_mixer_channel_mute_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::mute>(
            channel_id);
}

auto
make_mixer_channel_solo_parameter_selector(mixer::channel_id const channel_id)
        -> selector<bool_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::solo>(
            channel_id);
}

auto
make_mixer_channel_peak_level_parameter_selector(
        mixer::channel_id const channel_id)
        -> selector<stereo_level_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::peak_level>(
            channel_id);
}

auto
make_mixer_channel_rms_level_parameter_selector(
        mixer::channel_id const channel_id)
        -> selector<stereo_level_parameter_id>
{
    return make_mixer_channel_member_selector<&mixer::channel::rms_level>(
            channel_id);
}

static auto
get_mixer_channel_aux_volume_parameter(mixer::channel const& channel)
        -> float_parameter_id
{
    auto it = channel.aux_sends->find(channel.aux);
    return it != channel.aux_sends->end() ? it->second.volume
                                          : float_parameter_id{};
}

auto
make_mixer_channel_aux_volume_parameter_selector(
        mixer::channel_id const channel_id) -> selector<float_parameter_id>
{
    return make_mixer_channel_member_selector<
            &get_mixer_channel_aux_volume_parameter>(channel_id);
}

static auto
get_mixer_channel_aux_enabled(mixer::channel const& channel) -> bool
{
    auto it = channel.aux_sends->find(channel.aux);
    return it != channel.aux_sends->end() ? it->second.enabled : false;
}

auto
make_mixer_channel_aux_enabled_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return make_mixer_channel_member_selector<&get_mixer_channel_aux_enabled>(
            channel_id);
}

auto
make_mixer_channel_can_toggle_aux_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    auto get = memo([channel_id](mixer::channels_t const& channels) {
        return mixer::can_toggle_aux(channels, channel_id);
    });

    return [get = std::move(get)](state const& st) {
        return get(st.mixer_state.channels);
    };
}

static auto
get_default_route_name(
        audio::bus_type const bus_type,
        mixer::io_socket const io_socket) -> boxed_string
{
    static boxed_string const s_none{"None"};
    static boxed_string const s_mix{"Mix"};
    static boxed_string const s_send{"Send..."};

    switch (io_socket)
    {
        case mixer::io_socket::in:
            return bus_type == audio::bus_type::stereo ? s_mix : s_none;

        case mixer::io_socket::out:
            return s_none;

        case mixer::io_socket::aux:
            return s_send;
    }

    BOOST_ASSERT(false);
    return boxed_string{};
}

auto
make_mixer_channel_default_route_name_selector(
        audio::bus_type const bus_type,
        mixer::io_socket const io_socket) -> selector<boxed_string>
{
    return boost::hof::always(get_default_route_name(bus_type, io_socket));
}

auto
make_mixer_channel_default_route_is_valid_selector(
        mixer::channel_id const channel_id,
        mixer::io_socket const io_socket) -> selector<bool>
{
    switch (io_socket)
    {
        case mixer::io_socket::in:
            return [channel_id, get = memo(&mixer::is_default_source_valid)](
                           state const& st) {
                return get(st.mixer_state.channels, channel_id);
            };

        default:
            return boost::hof::always(true);
    }
}

static struct
{
    auto operator()(
            mixer::channel_id const channel_id,
            mixer::io_socket const io_socket,
            mixer::channels_t const& channels,
            external_audio::devices_t const& devices) const
            -> box<selected_route>
    {
        mixer::channel const* const mixer_channel = channels.find(channel_id);
        if (!mixer_channel)
        {
            static boxed_string const s_empty{"-"};
            return box_(
                    selected_route{selected_route::state_t::invalid, s_empty});
        }

        auto const default_name =
                get_default_route_name(mixer_channel->bus_type, io_socket);

        return box_(std::visit(
                boost::hof::match(
                        [&](default_t) {
                            return selected_route{
                                    .state = selected_route::state_t::valid,
                                    .name = default_name};
                        },
                        [&](mixer::channel_id const target_channel_id) {
                            if (mixer::channel const* const target_channel =
                                        channels.find(target_channel_id))
                            {
                                return selected_route{
                                        .state =
                                                io_socket == mixer::io_socket::
                                                                        in
                                                        ? selected_route::
                                                                  state_t::valid
                                                : std::holds_alternative<
                                                          default_t>(
                                                          target_channel->in)
                                                        ? selected_route::
                                                                  state_t::valid
                                                        : selected_route::state_t::
                                                                  not_mixed,
                                        .name = target_channel->name};
                            }
                            else
                            {
                                return selected_route{
                                        .state = selected_route::state_t::
                                                invalid,
                                        .name = default_name};
                            }
                        },
                        [&](external_audio::device_id const device_id) {
                            if (auto const* const device =
                                        devices.find(device_id);
                                device)
                            {
                                return selected_route{
                                        .state = selected_route::state_t::valid,
                                        .name = device->name};
                            }
                            else
                            {
                                return selected_route{
                                        .state = selected_route::state_t::
                                                invalid,
                                        .name = default_name};
                            }
                        },
                        [](mixer::missing_device_address const& missing) {
                            return selected_route{
                                    .state = selected_route::state_t::invalid,
                                    .name = missing.name};
                        }),
                mixer_channel->get_io_addr(io_socket)));
    }

    auto operator()(
            mixer::channel_id const channel_id,
            mixer::io_socket const io_socket) const
    {
        return [=,
                this](mixer::channels_t const& channels,
                      external_audio::devices_t const& devices) {
            return (*this)(channel_id, io_socket, channels, devices);
        };
    }
} const get_mixer_channel_selected_route;

auto
make_mixer_channel_selected_route_selector(
        mixer::channel_id const channel_id,
        mixer::io_socket const io_socket) -> selector<box<selected_route>>
{
    return [get = memo(
                    get_mixer_channel_selected_route(channel_id, io_socket))](
                   state const& st) {
        return get(st.mixer_state.channels, st.external_audio_state.devices);
    };
}

auto
make_mixer_device_routes_selector(
        audio::bus_type const bus_type,
        mixer::io_socket const io_socket)
        -> selector<boxed_vector<mixer_device_route>>
{
    auto get_mixer_device_routes = memo(
            [bus_type](
                    external_audio::devices_t const& devices,
                    unique_box<external_audio::device_ids_t> const& device_ids)
                    -> boxed_vector<mixer_device_route> {
                std::vector<mixer_device_route> result;
                for (auto device_id : *device_ids)
                {
                    if (auto const* const device = devices.find(device_id);
                        device && device->bus_type == bus_type)
                    {
                        result.emplace_back(mixer_device_route{
                                .device_id = device_id,
                                .name = *device->name});
                    }
                }
                return box_(std::move(result));
            });

    switch (io_socket)
    {
        case mixer::io_socket::in:
            return [get = std::move(get_mixer_device_routes)](
                           state const& st) mutable {
                return get(
                        st.external_audio_state.devices,
                        st.external_audio_state.inputs);
            };

        case mixer::io_socket::out:
        case mixer::io_socket::aux:
            BOOST_ASSERT(bus_type == audio::bus_type::stereo);
            return [get = std::move(get_mixer_device_routes)](
                           state const& st) mutable {
                return get(
                        st.external_audio_state.devices,
                        st.external_audio_state.outputs);
            };
    }

    BOOST_ASSERT(false);
    return boost::hof::always(boxed_vector<mixer_device_route>{});
}

auto
make_mixer_channel_routes_selector(
        mixer::channel_id const channel_id,
        mixer::io_socket const io_socket)
        -> selector<boxed_vector<mixer_channel_route>>
{
    if (io_socket == mixer::io_socket::aux)
    {
        auto get_mixer_channel_routes =
                memo([channel_id](mixer::channels_t const& channels) {
                    std::vector<mixer_channel_route> result;
                    for (auto const& [id, channel] : channels)
                    {
                        if (channel_id == id ||
                            channel.bus_type == audio::bus_type::mono)
                        {
                            continue;
                        }

                        result.emplace_back(id, channel.name);
                    }

                    return box_(std::move(result));
                });

        return [get = std::move(get_mixer_channel_routes)](state const& st) {
            return get(st.mixer_state.channels);
        };
    }
    else
    {
        auto get_mixer_channel_routes = memo(
                [channel_id, io_socket](mixer::channels_t const& channels) {
                    auto valid_sources = mixer::valid_channels(
                            io_socket,
                            channels,
                            channel_id);
                    return box_(algorithm::transform_to_vector(
                            valid_sources,
                            [&](auto const& id) {
                                return mixer_channel_route{
                                        .channel_id = id,
                                        .name = channels[id].name};
                            }));
                });

        return [get = std::move(get_mixer_channel_routes)](state const& st) {
            return get(st.mixer_state.channels);
        };
    }
}

auto
make_mixer_channel_name_selector(mixer::channel_id const channel_id)
        -> selector<boxed_string>
{
    return make_mixer_channel_member_selector<&mixer::channel::name>(
            channel_id);
}

auto
make_mixer_channel_can_move_left_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return [channel_id](state const& st) {
        return channel_id != st.mixer_state.main &&
               !st.mixer_state.inputs->empty() &&
               channel_id != st.mixer_state.inputs->front();
    };
}

auto
make_mixer_channel_can_move_right_selector(mixer::channel_id const channel_id)
        -> selector<bool>
{
    return [channel_id](state const& st) {
        return channel_id != st.mixer_state.main &&
               !st.mixer_state.inputs->empty() &&
               channel_id != st.mixer_state.inputs->back();
    };
}

selector<unique_box<midi::device_ids_t>> const select_midi_input_devices(
        [](state const& st) { return st.midi_inputs; });

auto
make_midi_device_name_selector(midi::device_id_t const device_id)
        -> selector<boxed_string>
{
    auto get_midi_device_name =
            memo([device_id](unique_box<midi_devices_t> const& midi_devices) {
                auto it = midi_devices->find(device_id);
                return it != midi_devices->end() ? it->second.name
                                                 : boxed_string();
            });

    return [get = std::move(get_midi_device_name)](state const& st) {
        return get(st.midi_devices);
    };
}

auto
make_midi_device_enabled_selector(midi::device_id_t const device_id)
        -> selector<bool>
{
    auto is_midi_device_enabled =
            memo([device_id](unique_box<midi_devices_t> const& midi_devices) {
                auto it = midi_devices->find(device_id);
                return it != midi_devices->end() && it->second.enabled;
            });

    return [get = std::move(is_midi_device_enabled)](state const& st) {
        return get(st.midi_devices);
    };
}

namespace
{

struct muted_by_solo_state
{
    muted_by_solo_state(
            mixer::channels_t const& channels,
            parameters_map const* const params) // non-null!
        : solo_groups{runtime::solo_groups(channels)}
        , solo_params{algorithm::transform_to_vector(
                  solo_groups | std::views::values,
                  [params](solo_group const& g) {
                      return params->find(g.solo_param)->value.cached();
                  })}
    {
    }

    auto operator()(
            std::size_t const reduce_count,
            mixer::channel_id const channel_id) const
    {
        if (last_reduce_count != reduce_count)
        {
            for (std::size_t index : range::indices(solo_params))
            {
                sg_state.set_solo(index, *solo_params[index]);
            }

            sg_state.calculate_mutes();
            last_reduce_count = reduce_count;
        }

        auto it = solo_groups.find(channel_id);
        BOOST_ASSERT(it != solo_groups.end());
        return sg_state.mutes().test(solo_groups.index_of(it));
    }

    solo_groups_t solo_groups;
    mutable solo_group_state sg_state{runtime::solo_group_state{solo_groups}};

    using cached_solo_param_value = std::shared_ptr<bool const>;
    std::vector<cached_solo_param_value> solo_params;

    mutable std::size_t last_reduce_count{piejam::npos};
};

auto
make_muted_by_solo_state(
        mixer::channels_t const& channels,
        parameters_map const* const params) -> unique_box<muted_by_solo_state>
{
    return unique_box_(muted_by_solo_state{channels, params});
}

selector<unique_box<muted_by_solo_state>> const select_muted_by_solo_state(
        [get = memo(&make_muted_by_solo_state)](state const& st) {
            return get(st.mixer_state.channels, &st.params);
        });

} // namespace

auto
make_muted_by_solo_selector(mixer::channel_id const mixer_channel_id)
        -> selector<bool>
{
    return [mixer_channel_id](state const& st) {
        auto muted_by_solo_state = select_muted_by_solo_state.get(st);
        return muted_by_solo_state.get()(st.reduce_count, mixer_channel_id);
    };
}

auto
make_fx_chain_selector(mixer::channel_id const channel_id)
        -> selector<unique_box<fx::chain_t>>
{
    return make_mixer_channel_member_selector<&mixer::channel::fx_chain>(
            channel_id);
}

template <auto GetMember>
auto
make_fx_module_member_selector(fx::module_id const fx_mod_id)
{
    static decltype(std::invoke(
            GetMember,
            std::declval<fx::module const*>())) s_default{};

    auto get = memo([fx_mod_id](fx::modules_t const& fx_modules) {
        fx::module const* const fx_mod = fx_modules.find(fx_mod_id);
        return fx_mod ? std::invoke(GetMember, fx_mod) : s_default;
    });

    return [get = std::move(get)](state const& st) {
        return get(st.fx_modules);
    };
}

auto
make_fx_module_instance_id_selector(fx::module_id const fx_mod_id)
        -> selector<fx::instance_id>
{
    return make_fx_module_member_selector<&fx::module::fx_instance_id>(
            fx_mod_id);
}

auto
make_fx_module_name_selector(fx::module_id const fx_mod_id)
        -> selector<boxed_string>
{
    return make_fx_module_member_selector<&fx::module::name>(fx_mod_id);
}

auto
make_fx_module_bus_type_selector(fx::module_id const fx_mod_id)
        -> selector<audio::bus_type>
{
    return make_fx_module_member_selector<&fx::module::bus_type>(fx_mod_id);
}

auto
make_fx_module_bypass_selector(fx::module_id const fx_mod_id) -> selector<bool>
{
    return make_fx_module_member_selector<&fx::module::bypassed>(fx_mod_id);
}

auto
make_fx_module_parameters_selector(fx::module_id const fx_mod_id)
        -> selector<unique_box<fx::module_parameters>>
{
    return make_fx_module_member_selector<&fx::module::parameters>(fx_mod_id);
}

static auto
make_fx_module_can_move_selector(mixer::channel_id const fx_chain_id, bool up)
        -> selector<bool>
{
    auto get = memo([fx_chain_id,
                     up](mixer::channels_t const& channels,
                         mixer::channel_id focused_fx_chain_id,
                         fx::module_id focused_fx_mod_id) {
        if (fx_chain_id != focused_fx_chain_id)
        {
            return false;
        }

        mixer::channel const* const mixer_channel = channels.find(fx_chain_id);
        return mixer_channel && !mixer_channel->fx_chain->empty() &&
               (up ? mixer_channel->fx_chain->front()
                   : mixer_channel->fx_chain->back()) != focused_fx_mod_id;
    });

    return [get = std::move(get)](state const& st) -> bool {
        return get(
                st.mixer_state.channels,
                st.gui_state.focused_fx_chain_id,
                st.gui_state.focused_fx_mod_id);
    };
}

auto
make_fx_module_can_move_up_selector(mixer::channel_id const fx_chain_id)
        -> selector<bool>
{
    return make_fx_module_can_move_selector(fx_chain_id, true);
}

auto
make_fx_module_can_move_down_selector(mixer::channel_id const fx_chain_id)
        -> selector<bool>
{
    return make_fx_module_can_move_selector(fx_chain_id, false);
}

auto
make_fx_parameter_name_selector(fx::parameter_id const fx_param_id)
        -> selector<boxed_string>
{
    static boxed_string const s_empty;
    return std::visit(
            [](auto param_id) -> selector<boxed_string> {
                return [param_id](state const& st) {
                    if (auto const* const desc = st.ui_params.find(param_id);
                        desc)
                    {
                        return desc->name;
                    }
                    return s_empty;
                };
            },
            fx_param_id);
}

auto
make_fx_parameter_value_string_selector(fx::parameter_id const fx_param_id)
        -> selector<std::string>
{
    return std::visit(
            [](auto param_id) -> selector<std::string> {
                using parameter_t = typename decltype(param_id)::tag;
                using cached_value_type = typename parameter_map_slot<
                        parameter_t>::value_slot::cached_type;
                using value_to_string_fn = typename ui_parameter_map_slot<
                        parameter_t>::value_to_string_fn;
                using memoed_value_to_string_fn =
                        decltype(memo(std::declval<value_to_string_fn>()));

                return [param_id,
                        cached_value = cached_value_type{},
                        value_to_string =
                                std::shared_ptr<memoed_value_to_string_fn>{}](
                               state const& st) mutable {
                    if (cached_value && value_to_string) [[likely]]
                    {
                        return (*value_to_string)(*cached_value);
                    }

                    auto const* const desc = st.params.find(param_id);
                    auto const* const ui_desc =
                            desc ? st.ui_params.find(param_id) : nullptr;

                    if (desc && ui_desc)
                    {
                        cached_value = desc->value.cached();
                        value_to_string =
                                std::make_shared<memoed_value_to_string_fn>(
                                        memo(ui_desc->value_to_string));

                        return ui_desc->value_to_string(desc->value.get());
                    }

                    return std::string{};
                };
            },
            fx_param_id);
}

auto
make_fx_module_streams_selector(fx::module_id fx_mod_id)
        -> selector<unique_box<fx::module_streams>>
{
    static unique_box<fx::module_streams> const s_empty;

    auto get_fx_module_streams =
            memo([fx_mod_id](fx::modules_t const& fx_modules) {
                fx::module const* const fx_mod = fx_modules.find(fx_mod_id);
                return fx_mod ? fx_mod->streams : s_empty;
            });

    return [get = std::move(get_fx_module_streams)](state const& st) {
        return get(st.fx_modules);
    };
}

auto
make_audio_stream_selector(audio_stream_id stream_id)
        -> selector<audio_stream_buffer>
{
    static audio_stream_buffer s_empty(std::in_place, 1);

    auto get_audio_stream =
            memo([stream_id](audio_streams_cache const& streams) {
                auto* stream = streams.find(stream_id);
                return stream ? *stream : s_empty;
            });

    return [get = std::move(get_audio_stream)](
                   state const& st) -> audio_stream_buffer {
        return get(st.streams);
    };
}

template <class P>
auto
make_parameter_value_selector(parameter::id_t<P> const param_id)
        -> selector<parameter::value_type_t<P>>
{
    return [param_id,
            value = std::shared_ptr<parameter::value_type_t<P> const>{}](
                   state const& st) mutable -> parameter::value_type_t<P> {
        if (value) [[likely]]
        {
            return *value;
        }

        if (auto const* const desc = st.params.find(param_id); desc)
        {
            value = desc->value.cached();
            BOOST_ASSERT(value);
            return *value;
        }

        return {};
    };
}

auto
make_bool_parameter_value_selector(bool_parameter_id const param_id)
        -> selector<bool>
{
    return make_parameter_value_selector(param_id);
}

auto
make_float_parameter_value_selector(float_parameter_id const param_id)
        -> selector<float>
{
    return make_parameter_value_selector(param_id);
}

auto
make_float_parameter_normalized_value_selector(
        float_parameter_id const param_id) -> selector<float>
{
    return [param_id](state const& st) -> float {
        if (auto const* const desc = st.params.find(param_id); desc)
        {
            float const value = desc->value.get();
            BOOST_ASSERT(desc->param.to_normalized);
            return desc->param.to_normalized(desc->param, value);
        }
        return 0.f;
    };
}

auto
make_int_parameter_value_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return make_parameter_value_selector(param_id);
}

auto
make_int_parameter_min_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const desc = st.params.find(param_id);
        return desc ? desc->param.min : 0;
    };
}

auto
make_int_parameter_max_selector(int_parameter_id const param_id)
        -> selector<int>
{
    return [param_id](state const& st) -> int {
        auto const* const desc = st.params.find(param_id);
        return desc ? desc->param.max : 1;
    };
}

auto
make_int_parameter_enum_values_selector(int_parameter_id const param_id)
        -> selector<std::vector<std::pair<std::string, int>>>
{
    return [param_id](state const& st) {
        std::vector<std::pair<std::string, int>> result;

        auto const* const desc = st.params.find(param_id);
        auto const* const ui_desc = st.ui_params.find(param_id);
        if (desc && ui_desc)
        {
            for (int value = desc->param.min; value <= desc->param.max; ++value)
            {
                result.emplace_back(ui_desc->value_to_string(value), value);
            }
        }

        return result;
    };
}

auto
make_level_parameter_value_selector(stereo_level_parameter_id const param_id)
        -> selector<stereo_level>
{
    return make_parameter_value_selector(param_id);
}

auto
make_midi_assignment_selector(midi_assignment_id const id)
        -> selector<std::optional<midi_assignment>>
{
    return [id](state const& st) -> std::optional<midi_assignment> {
        auto it = st.midi_assignments->find(id);
        return it != st.midi_assignments->end()
                       ? it->second
                       : std::optional<midi_assignment>{};
    };
}

selector<bool> const select_midi_learning([](state const& st) {
    return st.midi_learning.has_value();
});

selector<fx::registry> const select_fx_registry([](state const& st) {
    return st.fx_registry;
});

selector<bool> const select_recording([](state const& st) {
    return st.recording;
});

selector<std::size_t> const select_xruns([](state const& st) {
    return st.xruns;
});

selector<float> const select_cpu_load([](state const& st) {
    return st.cpu_load;
});

selector<root_view_mode> const select_root_view_mode([](state const& st) {
    return st.gui_state.root_view_mode_;
});

selector<mixer::channel_id> const select_fx_browser_fx_chain(
        [](state const& st) { return st.gui_state.fx_browser_fx_chain_id; });

selector<mixer::channel_id> const select_focused_fx_chain([](state const& st) {
    return st.gui_state.focused_fx_chain_id;
});

selector<fx::module_id> const select_focused_fx_module([](state const& st) {
    return st.gui_state.focused_fx_mod_id;
});

static auto
get_focused_fx_module_name(
        fx::modules_t const& fx_modules,
        fx::module_id const focused_fx_mod_id)
{
    static boxed_string s_empty_name;

    fx::module const* const fx_mod = fx_modules.find(focused_fx_mod_id);
    return fx_mod ? fx_mod->name : s_empty_name;
}

selector<boxed_string> const select_focused_fx_module_name(
        [get = memo(&get_focused_fx_module_name)](state const& st) {
            return get(st.fx_modules, st.gui_state.focused_fx_mod_id);
        });

static auto
get_focused_fx_module_bypassed(
        fx::modules_t const& fx_modules,
        fx::module_id const focused_fx_mod_id)
{
    fx::module const* const fx_mod = fx_modules.find(focused_fx_mod_id);
    return fx_mod && fx_mod->bypassed;
}

selector<bool> const select_focused_fx_module_bypassed(
        [get = memo(&get_focused_fx_module_bypassed)](state const& st) -> bool {
            return get(st.fx_modules, st.gui_state.focused_fx_mod_id);
        });

} // namespace piejam::runtime::selectors
