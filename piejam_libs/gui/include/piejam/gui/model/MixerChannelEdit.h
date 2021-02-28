// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <piejam/gui/model/SubscribableModel.h>
#include <piejam/gui/model/fwd.h>

#include <QStringList>

namespace piejam::gui::model
{

class MixerChannelEdit : public SubscribableModel
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged FINAL)

    Q_PROPERTY(bool defaultInputIsValid READ defaultInputIsValid NOTIFY
                       defaultInputIsValidChanged FINAL)
    Q_PROPERTY(bool selectedInputIsValid READ selectedInputIsValid NOTIFY
                       selectedInputIsValidChanged FINAL)
    Q_PROPERTY(QString selectedInput READ selectedInput NOTIFY
                       selectedInputChanged FINAL)
    Q_PROPERTY(QStringList inputDevices READ inputDevices NOTIFY
                       inputDevicesChanged FINAL)
    Q_PROPERTY(QStringList inputChannels READ inputChannels NOTIFY
                       inputChannelsChanged FINAL)
    Q_PROPERTY(bool selectedOutputIsValid READ selectedOutputIsValid NOTIFY
                       selectedOutputIsValidChanged FINAL)
    Q_PROPERTY(QString selectedOutput READ selectedOutput NOTIFY
                       selectedOutputChanged FINAL)
    Q_PROPERTY(QStringList outputDevices READ outputDevices NOTIFY
                       outputDevicesChanged FINAL)
    Q_PROPERTY(QStringList outputChannels READ outputChannels NOTIFY
                       outputChannelsChanged FINAL)

public:
    using SubscribableModel::SubscribableModel;

    auto name() const noexcept -> QString const& { return m_name; }
    void setName(QString const& x)
    {
        if (m_name != x)
        {
            m_name = x;
            emit nameChanged();
        }
    }

    Q_INVOKABLE virtual void changeName(QString const&) = 0;

    Q_INVOKABLE virtual void deleteChannel() = 0;

    bool defaultInputIsValid() const noexcept { return m_defaultInputIsValid; }
    void setDefaultInputIsValid(bool x)
    {
        if (m_defaultInputIsValid != x)
        {
            m_defaultInputIsValid = x;
            emit defaultInputIsValidChanged();
        }
    }

    bool selectedInputIsValid() const noexcept
    {
        return m_selectedInputIsValid;
    }

    void setSelectedInputIsValid(bool x)
    {
        if (m_selectedInputIsValid != x)
        {
            m_selectedInputIsValid = x;
            emit selectedInputIsValidChanged();
        }
    }

    auto selectedInput() const noexcept -> QString const&
    {
        return m_selectedInput;
    }

    void setSelectedInput(QString const& x)
    {
        if (m_selectedInput != x)
        {
            m_selectedInput = x;
            emit selectedInputChanged();
        }
    }

    auto inputDevices() const -> QStringList const& { return m_inputDevices; }
    void setInputDevices(QStringList const& x)
    {
        if (m_inputDevices != x)
        {
            m_inputDevices = x;
            emit inputDevicesChanged();
        }
    }

    auto inputChannels() const -> QStringList const& { return m_inputChannels; }
    void setInputChannels(QStringList const& x)
    {
        if (m_inputChannels != x)
        {
            m_inputChannels = x;
            emit inputChannelsChanged();
        }
    }

    Q_INVOKABLE virtual void changeInputToMix() = 0;
    Q_INVOKABLE virtual void changeInputToDevice(unsigned index) = 0;
    Q_INVOKABLE virtual void changeInputToChannel(unsigned index) = 0;

    bool selectedOutputIsValid() const noexcept
    {
        return m_selectedOutputIsValid;
    }

    void setSelectedOutputIsValid(bool x)
    {
        if (m_selectedOutputIsValid != x)
        {
            m_selectedOutputIsValid = x;
            emit selectedOutputIsValidChanged();
        }
    }

    auto selectedOutput() const noexcept -> QString const&
    {
        return m_selectedOutput;
    }

    void setSelectedOutput(QString const& x)
    {
        if (m_selectedOutput != x)
        {
            m_selectedOutput = x;
            emit selectedOutputChanged();
        }
    }

    auto outputDevices() const -> QStringList const& { return m_outputDevices; }
    void setOutputDevices(QStringList const& x)
    {
        if (m_outputDevices != x)
        {
            m_outputDevices = x;
            emit outputDevicesChanged();
        }
    }

    auto outputChannels() const -> QStringList const&
    {
        return m_outputChannels;
    }

    void setOutputChannels(QStringList const& x)
    {
        if (m_outputChannels != x)
        {
            m_outputChannels = x;
            emit outputChannelsChanged();
        }
    }

    Q_INVOKABLE virtual void changeOutputToNone() = 0;
    Q_INVOKABLE virtual void changeOutputToDevice(unsigned index) = 0;
    Q_INVOKABLE virtual void changeOutputToChannel(unsigned index) = 0;

signals:

    void nameChanged();
    void defaultInputIsValidChanged();
    void selectedInputIsValidChanged();
    void selectedInputChanged();
    void inputDevicesChanged();
    void inputChannelsChanged();
    void selectedOutputIsValidChanged();
    void selectedOutputChanged();
    void outputDevicesChanged();
    void outputChannelsChanged();

private:
    QString m_name;
    bool m_defaultInputIsValid{};
    bool m_selectedInputIsValid{};
    QString m_selectedInput;
    QStringList m_inputDevices;
    QStringList m_inputChannels;
    bool m_selectedOutputIsValid{};
    QString m_selectedOutput;
    QStringList m_outputDevices;
    QStringList m_outputChannels;
};

} // namespace piejam::gui::model
