// PieJam - An audio mixer for Raspberry Pi.
//
// Copyright (C) 2020  Dimitrij Kotrev
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

#pragma once

#include <piejam/gui/model/MixerChannel.h>
#include <piejam/gui/model/MixerChannelsList.h>

#include <QObject>

#include <memory>
#include <vector>

namespace piejam::gui::model
{

class Mixer : public QObject
{
    Q_OBJECT

    Q_PROPERTY(MixerChannelsList* inputChannels READ inputChannels CONSTANT)
    Q_PROPERTY(MixerChannelsList* outputChannels READ outputChannels CONSTANT)

public:
    Q_INVOKABLE virtual void subscribe() = 0;
    Q_INVOKABLE virtual void unsubscribe() = 0;

    auto inputChannels() -> MixerChannelsList* { return &m_inputChannels; }
    auto numInputChannels() const noexcept -> std::size_t
    {
        return m_inputChannels.rowCount();
    }

    auto outputChannels() -> MixerChannelsList* { return &m_outputChannels; }
    auto numOutputChannels() const noexcept -> std::size_t
    {
        return m_outputChannels.rowCount();
    }

    virtual Q_INVOKABLE void
    setInputChannelVolume(unsigned index, double volume) = 0;
    virtual Q_INVOKABLE void setInputChannelPan(unsigned index, double pan) = 0;
    virtual Q_INVOKABLE void setInputChannelMute(unsigned index, bool mute) = 0;
    virtual Q_INVOKABLE void
    setOutputChannelVolume(unsigned index, double volume) = 0;
    virtual Q_INVOKABLE void
    setOutputChannelBalance(unsigned index, double balance) = 0;
    virtual Q_INVOKABLE void
    setOutputChannelMute(unsigned index, bool mute) = 0;
    virtual Q_INVOKABLE void requestLevelsUpdate() = 0;

private:
    MixerChannelsList m_inputChannels;
    MixerChannelsList m_outputChannels;
};

} // namespace piejam::gui::model
