// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import ".."
import "../Controls"

Item {
    property alias name: title.text
    property bool mono: false
    property alias levelLeft: levelMeterFader.levelLeft
    property alias levelRight: levelMeterFader.levelRight
    property alias meterMuted: levelMeterFader.muted
    property alias pan: panControls.value
    property alias volume: levelMeterFader.volume
    property alias mute: channelControls.mute
    property alias solo: channelControls.solo

    property alias panMidi: midiAssignPan
    property alias volumeMidi: levelMeterFader.volumeMidi
    property alias muteMidi: channelControls.muteMidi

    signal faderMoved(real newVolume)
    signal panMoved(real newPan)
    signal muteToggled()
    signal soloToggled()
    signal fxButtonClicked()

    id: root

    implicitWidth: 150
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        HeaderLabel {
            id: title
            width: 96

            height: 24

            anchors.top: parent.top
            anchors.left: parent.left
        }

        BipolarSlider {
            id: panControls

            height: 40
            anchors.right: parent.right
            anchors.left: parent.left
            anchors.top: title.bottom

            onMoved: {
                root.panMoved(panControls.value)
                Info.quickTip =
                        (root.mono ? "<b>Pan:</b> " : "<b>Balance:</b> ") + panControls.value.toFixed(2)
            }

            MidiAssignArea {
                id: midiAssignPan

                anchors.fill: parent
                anchors.topMargin: 1
                anchors.bottomMargin: 1
            }
        }

        LevelMeterFader {
            id: levelMeterFader

            anchors.right: parent.right
            anchors.left: parent.left
            anchors.bottom: channelControls.top
            anchors.top: panControls.bottom

            onFaderMoved: root.faderMoved(newVolume)
        }

        ChannelControls {
            id: channelControls
            height: 32
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            onMuteToggled: root.muteToggled()
            onSoloToggled: root.soloToggled()
        }

        Button {
            id: fxButton

            width: 24
            height: 36

            anchors.right: parent.right
            anchors.rightMargin: 0
            anchors.top: parent.top
            anchors.topMargin: -6

            text: qsTr("fx")

            padding: 6

            onClicked: root.fxButtonClicked()
        }
    }
}