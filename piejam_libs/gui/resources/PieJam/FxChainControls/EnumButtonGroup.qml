// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
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
    property var icons: null
    property int alignment: Qt.Horizontal
    property int spacing: 5

    QtObject {
        id: private_

        readonly property var paramModel: root.paramModel && root.paramModel.type === FxParameter.Type.Enum ? root.paramModel : null
    }

    GridLayout {
        anchors.fill: parent

        columns: root.alignment === Qt.Vertical ? 1 : Number.MAX_VALUE
        rows: root.alignment === Qt.Horizontal ? 1 : Number.MAX_VALUE
        columnSpacing: root.spacing
        rowSpacing: root.spacing

        Repeater {
            model: private_.paramModel ? private_.paramModel.values : null

            delegate: Button {
                Layout.fillWidth: true
                Layout.fillHeight: true

                text: model.text

                checkable: true
                autoExclusive: true

                icon.source: root.icons ? root.icons[model.index] : ""

                display: icons ? AbstractButton.IconOnly : AbstractButton.TextOnly

                checked: model.value === private_.paramModel.value

                onClicked: {
                    private_.paramModel.changeValue(model.value)
                    Info.quickTip = "<b>" + private_.paramModel.name + "</b>: " + private_.paramModel.valueString
                }
            }
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
