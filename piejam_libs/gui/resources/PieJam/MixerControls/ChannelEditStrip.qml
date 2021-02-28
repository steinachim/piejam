// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2021  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15
import QtQuick.Layouts 1.15

import QtQml 2.15

import ".."

Item {
    id: root

    property var model
    property bool deletable: true

    implicitWidth: 150
    implicitHeight: 400

    Frame {
        id: frame

        anchors.fill: parent

        Button {
            id: deleteButton

            visible: root.deletable
            enabled: root.deletable

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

            text: qsTr("Delete")

            Material.background: Material.Red

            onClicked: root.model.deleteChannel()
        }

        TextField {
            id: nameText

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            text: root.model ? root.model.name : ""

            placeholderText: qsTr("Name")

            onTextEdited: root.model.changeName(nameText.text)
        }

        Label {
            id: audioInLabel

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: moveButtonsRow.bottom
            anchors.topMargin: 8

            text: qsTr("Audio In")
        }

        ComboBox {
            id: audioInSelect

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: audioInLabel.bottom

            displayText: root.model ? root.model.selectedInput : ""

            Material.foreground: (root.model ? root.model.selectedInputIsValid : true)
                                        ? Material.primaryTextColor
                                        : Material.Red

            popup: Menu {
                id: audioInMenu

                MenuItem {
                    enabled: root.model ? root.model.defaultInputIsValid : false

                    text: qsTr("Mix")

                    onClicked: root.model.changeInputToMix()
                }

                Menu {
                    title: qsTr("Devices")

                    enabled: inDevicesRep.count > 0

                    Repeater {
                        id: inDevicesRep

                        model: root.model ? root.model.inputDevices : []

                        delegate: MenuItem {
                            text: modelData

                            onClicked: root.model.changeInputToDevice(index)
                        }
                    }
                }

                Menu {
                    title: qsTr("Channels")

                    enabled: inChannelsRep.count > 0

                    Repeater {
                        id: inChannelsRep

                        model: root.model ? root.model.inputChannels : []

                        delegate: MenuItem {
                            text: modelData

                            onClicked: root.model.changeInputToChannel(index)
                        }
                    }
                }
            }
        }

        Label {
            id: audioOutLabel

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: audioInSelect.bottom
            anchors.topMargin: 8

            text: qsTr("Audio Out")
        }

        ComboBox {
            id: audioOutSelect

            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: audioOutLabel.bottom

            displayText: root.model ? root.model.selectedOutput : ""

            Material.foreground: (root.model ? root.model.selectedOutputIsValid : true)
                                        ? Material.primaryTextColor
                                        : Material.Red

            popup: Menu {
                id: audioOutMenu

                MenuItem {
                    text: qsTr("None")

                    onClicked: root.model.changeOutputToNone()
                }

                Menu {
                    title: qsTr("Devices")

                    enabled: outDevicesRep.count > 0

                    Repeater {
                        id: outDevicesRep

                        model: root.model ? root.model.outputDevices : []

                        delegate: MenuItem {
                            text: modelData

                            onClicked: root.model.changeOutputToDevice(index)
                        }
                    }
                }

                Menu {
                    title: qsTr("Channels")

                    enabled: outChannelsRep.count > 0

                    Repeater {
                        id: outChannelsRep

                        model: root.model ? root.model.outputChannels : []

                        delegate: MenuItem {
                            text: modelData

                            onClicked: root.model.changeOutputToChannel(index)
                        }
                    }
                }
            }
        }

        Row {
            id: moveButtonsRow

            height: 48

            anchors.top: nameText.bottom
            anchors.horizontalCenter: parent.horizontalCenter

            spacing: 8

            Button {
                id: moveLeftButton

                width: 48
                height: 48

                text: qsTr("<")
            }

            Button {
                id: moveRightButton

                width: 48
                height: 48

                text: qsTr(">")
            }
        }
    }

    Binding {
        when: root.model
        target: root.model
        property: "subscribed"
        value: root.visible
        restoreMode: Binding.RestoreBinding
    }
}
