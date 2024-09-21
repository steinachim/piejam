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
import "../Util/MathExt.js" as MathExt

SubscribableItem {
    id: root

    QtObject {
        id: private_

        readonly property var paramModel: root.model && root.model.type === Parameter.Type.Float ? root.model : null
        readonly property real value: private_.paramModel ? private_.paramModel.normalizedValue : 0
        readonly property bool bipolar: private_.paramModel ? private_.paramModel.bipolar : false
    }

    ColumnLayout {
        anchors.fill: parent

        spacing: 0

        RowLayout {
            spacing: 2

            Rectangle {
                color: Material.backgroundColor

                Layout.preferredWidth: 0.10 * parent.width
                Layout.fillHeight: true
                Layout.leftMargin: 2
                Layout.topMargin: 2
                Layout.bottomMargin: 2

                Rectangle {
                    id: indicator

                    color: Material.accentColor

                    x: 0
                    y: (private_.bipolar ? Math.min(0.5, 1 - private_.value) : (1 - private_.value)) * parent.height
                    width: parent.width
                    height: (private_.bipolar ? Math.abs(private_.value - 0.5) : private_.value) * parent.height
                }
            }

            Touchstrip {
                value: private_.value

                resolution: resolutionButton.checked ? 1000 : 250

                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.rightMargin: 2
                Layout.topMargin: 2
                Layout.bottomMargin: 2

                onChangeValue: {
                    if (private_.paramModel) {
                        private_.paramModel.changeNormalizedValue(newValue)
                        Info.showParameterValue(private_.paramModel)
                    }
                }
            }

            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Button {
            id: resolutionButton

            checkable: true
            display: AbstractButton.IconOnly
            icon.source: "qrc:///images/icons/arrow-expand-vertical.svg"

            Layout.leftMargin: 2
            Layout.rightMargin: 2
            Layout.fillWidth: true
            Layout.maximumHeight: 40
        }
    }

    MidiAssignArea {
        anchors.fill: parent

        model: private_.paramModel ? private_.paramModel.midi : null
    }
}
