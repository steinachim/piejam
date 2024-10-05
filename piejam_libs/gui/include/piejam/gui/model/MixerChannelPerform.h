// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020-2024  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/PropertyMacros.h>
#include <piejam/gui/model/MixerChannel.h>

#include <piejam/audio/types.h>
#include <piejam/runtime/external_audio_fwd.h>
#include <piejam/runtime/mixer_fwd.h>
#include <piejam/runtime/parameters.h>

#include <memory>

namespace piejam::gui::model
{

class MixerChannelPerform final : public MixerChannel
{
    Q_OBJECT

    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::FloatParameter*, volume)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::StereoLevelParameter*,
            peakLevel)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::StereoLevelParameter*,
            rmsLevel)
    M_PIEJAM_GUI_READONLY_PROPERTY(
            piejam::gui::model::FloatParameter*,
            panBalance)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, record)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, solo)
    M_PIEJAM_GUI_READONLY_PROPERTY(piejam::gui::model::BoolParameter*, mute)
    M_PIEJAM_GUI_PROPERTY(bool, mutedBySolo, setMutedBySolo)

public:
    MixerChannelPerform(
            runtime::store_dispatch,
            runtime::subscriber&,
            runtime::mixer::channel_id);
    ~MixerChannelPerform() override;

private:
    void onSubscribe() override;

    struct Impl;
    std::unique_ptr<Impl> m_impl;
};

} // namespace piejam::gui::model
