// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2023  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import PieJam.Models 1.0

import ".."
import "../Controls"

Item {
    id: root

    property var paramModel: null
    property real stepScale: 1

    implicitWidth: 96
    implicitHeight: 40

    QtObject {
        id: private_

        readonly property var paramModel: root.paramModel && root.paramModel.type === FxParameter.Type.Float ? root.paramModel : null
        readonly property string valueString: private_.paramModel ? private_.paramModel.valueString : "#"
    }

    QuickSpinBox {
        from: 0
        to: 10000
        step: 100 * root.stepScale
        bigStep: 500 * root.stepScale
        value: private_.paramModel ? private_.paramModel.normalizedValue * 10000 : 0

        textFromValue: function(value) {
            return private_.valueString
        }

        anchors.fill: parent

        onChangeValue: {
            if (private_.paramModel) {
                private_.paramModel.changeNormalizedValue(newValue / 10000)
                Info.quickTip = "<b>" + private_.paramModel.name + "</b>: " + private_.valueString
            }
        }

        onReset: {
            if (private_.paramModel) {
                private_.paramModel.resetToDefault()
                Info.quickTip = "<b>" + private_.paramModel.name + "</b>: " + private_.valueString
            }
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: root.paramModel ? root.paramModel.midi : null
    }
}
