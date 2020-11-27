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

import QtQuick 2.13
import QtQuick.Controls 2.13
import QtQuick.Controls.Material 2.13
import QtQuick.Layouts 1.13

Item {
    id: root

    property alias name: nameLabel.text
    property alias parameters: parametersList.model

    signal deleteButtonClicked()

    implicitWidth: frame.implicitWidth
    implicitHeight: frame.implicitHeight

    Frame {
        id: frame

        anchors.top: parent.top
        anchors.bottom: parent.bottom

        implicitWidth: Math.max(nameLabel.implicitWidth + deleteButton.width + 8, parametersList.implicitWidth) + 2 * frame.padding
        implicitHeight: nameLabel.implicitHeight + parametersList.implicitHeight + 2 * frame.padding

        Label {
            id: nameLabel

            padding: 2
            verticalAlignment: Text.AlignVCenter
            background: Rectangle {
                color: Material.primaryColor
                radius: 2
            }
            font.bold: true
            font.pixelSize: 12
        }

        Button {
            id: deleteButton
            width: 24
            height: 34

            text: "x"
            anchors.right: parent.right
            anchors.top: nameLabel.top
            font.bold: true
            font.pixelSize: 12
            anchors.topMargin: -8

            onClicked: deleteButtonClicked()
        }

        ListView {
            id: parametersList
            anchors.top: nameLabel.bottom
            anchors.bottom: parent.bottom

            orientation: Qt.Horizontal

            spacing: 4

            implicitWidth: contentWidth

            delegate: ParameterControl {
                name: modelData.name
                value: modelData.value
                valueText: modelData.valueText

                height: parametersList.height
            }
        }
    }
}