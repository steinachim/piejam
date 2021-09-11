// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#include <piejam/gui/model/MixerChannelPerform.h>

#include <piejam/gui/model/DbScaleData.h>
#include <piejam/gui/model/MidiAssignable.h>
#include <piejam/gui/model/MixerDbScales.h>
#include <piejam/math.h>
#include <piejam/runtime/actions/fwd.h>
#include <piejam/runtime/actions/set_parameter_value.h>
#include <piejam/runtime/parameter/float_.h>
#include <piejam/runtime/parameter/generic_value.h>
#include <piejam/runtime/parameter/int_.h>
#include <piejam/runtime/selectors.h>

namespace piejam::gui::model
{

struct MixerChannelPerform::Impl
{
    runtime::mixer::channel_id busId;
    runtime::float_parameter_id volume;
    runtime::float_parameter_id panBalance;
    runtime::bool_parameter_id record;
    runtime::bool_parameter_id mute;
    runtime::bool_parameter_id solo;
    runtime::stereo_level_parameter_id level;

    std::unique_ptr<MidiAssignable> volumeMidi;
    std::unique_ptr<MidiAssignable> panMidi;
    std::unique_ptr<MidiAssignable> muteMidi;
    std::unique_ptr<MidiAssignable> soloMidi;
};

MixerChannelPerform::MixerChannelPerform(
        runtime::store_dispatch store_dispatch,
        runtime::subscriber& state_change_subscriber,
        runtime::mixer::channel_id const id)
    : Subscribable(store_dispatch, state_change_subscriber)
    , m_impl(std::make_unique<Impl>(
              id,
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_volume_parameter_selector(id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_pan_balance_parameter_selector(
                                      id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_record_parameter_selector(id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_mute_parameter_selector(id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_solo_parameter_selector(id)),
              observe_once(
                      runtime::selectors::
                              make_mixer_channel_level_parameter_selector(id)),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_volume_parameter_selector(
                                              id))),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_pan_balance_parameter_selector(
                                              id))),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_mute_parameter_selector(
                                              id))),
              std::make_unique<MidiAssignable>(
                      store_dispatch,
                      state_change_subscriber,
                      observe_once(
                              runtime::selectors::
                                      make_mixer_channel_solo_parameter_selector(
                                              id)))))
{
}

MixerChannelPerform::~MixerChannelPerform() = default;

static constexpr auto toLevel =
        [min = math::dB_to_linear(-60.01f)](float const l) {
            return math::linear_to_dB(l, min);
        };

static constexpr auto toVolume =
        [min = math::dB_to_linear(-60.01f)](float const v) {
            return math::linear_to_dB(v, min);
        };

void
MixerChannelPerform::onSubscribe()
{
    observe(runtime::selectors::make_mixer_channel_name_selector(m_impl->busId),
            [this](boxed_string const& name) {
                setName(QString::fromStdString(*name));
            });

    observe(runtime::selectors::make_mixer_channel_input_type_selector(
                    m_impl->busId),
            [this](audio::bus_type const bus_type) {
                setMono(bus_type == audio::bus_type::mono);
            });

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_impl->volume),
            [this](float x) {
                setVolume(g_mixerDbScales.volumeFaderScale()->dBToPosition(
                        toVolume(x)));
            });

    observe(runtime::selectors::make_float_parameter_value_selector(
                    m_impl->panBalance),
            [this](float x) { setPanBalance(x); });

    observe(runtime::selectors::make_bool_parameter_value_selector(
                    m_impl->record),
            [this](bool x) { setRecord(x); });

    observe(runtime::selectors::make_bool_parameter_value_selector(
                    m_impl->mute),
            [this](bool x) { setMute(x); });

    observe(runtime::selectors::make_bool_parameter_value_selector(
                    m_impl->solo),
            [this](bool x) { setSolo(x); });

    observe(runtime::selectors::make_muted_by_solo_selector(m_impl->busId),
            [this](bool x) { setMutedBySolo(x); });

    observe(runtime::selectors::make_level_parameter_value_selector(
                    m_impl->level),
            [this](runtime::stereo_level const& x) {
                setLevel(
                        g_mixerDbScales.levelMeterScale()->dBToPosition(
                                toLevel(x.left)),
                        g_mixerDbScales.levelMeterScale()->dBToPosition(
                                toLevel(x.right)));
            });
}

void
MixerChannelPerform::changeVolume(double position)
{
    dispatch(runtime::actions::set_float_parameter(
            m_impl->volume,
            static_cast<float>(math::dB_to_linear(
                    g_mixerDbScales.volumeFaderScale()->dBAt(position)))));
}

void
MixerChannelPerform::changePanBalance(double value)
{
    dispatch(runtime::actions::set_float_parameter(
            m_impl->panBalance,
            static_cast<float>(value)));
}

void
MixerChannelPerform::changeRecord(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_impl->record, value));
}

void
MixerChannelPerform::changeMute(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_impl->mute, value));
}

void
MixerChannelPerform::changeSolo(bool value)
{
    dispatch(runtime::actions::set_bool_parameter(m_impl->solo, value));
}

auto
MixerChannelPerform::volumeMidi() const noexcept -> MidiAssignable*
{
    return m_impl->volumeMidi.get();
}

auto
MixerChannelPerform::panMidi() const noexcept -> MidiAssignable*
{
    return m_impl->panMidi.get();
}

auto
MixerChannelPerform::muteMidi() const noexcept -> MidiAssignable*
{
    return m_impl->muteMidi.get();
}

auto
MixerChannelPerform::soloMidi() const noexcept -> MidiAssignable*
{
    return m_impl->soloMidi.get();
}

} // namespace piejam::gui::model
