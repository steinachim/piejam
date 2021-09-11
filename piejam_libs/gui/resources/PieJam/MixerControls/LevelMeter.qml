// PieJam - An audio mixer for Raspberry Pi.
// SPDX-FileCopyrightText: 2020  Dimitrij Kotrev
// SPDX-License-Identifier: GPL-3.0-or-later

import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.15

Item {
    id: root

    property real levelLeft: 0
    property real levelRight: 0
    property bool muted: false
    property var scaleData

    implicitWidth: dbScaleLeft.width + indicatorLeft.width + indicatorRight.width + dbScaleRight.width + 3 * controls.spacing
    implicitHeight: 300

    Rectangle {
        anchors.fill: parent
        color: Material.backgroundColor
    }

    Gradient {
        id: levelGradient

        GradientStop { position: 0; color: "#ff0000" }
        GradientStop { position: 0.0475; color: "#ffa500" }
        GradientStop { position: 0.19; color: "#ffff00" }
        GradientStop { position: 0.285; color: "#7ee00d" }
        GradientStop { position: 0.95; color: "#7ee00d" }
        GradientStop { position: 1; color: "#008000" }
    }

    Gradient {
        id: mutedLevelGradient

        GradientStop { position: 0; color: "#c0c0c0" }
        GradientStop { position: 1; color: "#909090" }
    }

    Row {
        id: controls

        anchors.fill: parent

        spacing: 1

        DbScale {
            id: dbScaleLeft

            height: parent.height

            padding: 6

            orientation: DbScale.Orientation.Right
            backgroundColor: Material.backgroundColor

            scaleData: root.scaleData
        }

        LevelIndicator {
            id: indicatorLeft

            width: 6

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 6

            level: root.levelLeft

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        LevelIndicator {
            id: indicatorRight

            width: 6

            anchors.bottom: parent.bottom
            anchors.bottomMargin: 6
            anchors.top: parent.top
            anchors.topMargin: 6

            level: root.levelRight

            gradient: root.muted ? mutedLevelGradient : levelGradient

            fillColor: Material.backgroundColor
        }

        DbScale {
            id: dbScaleRight

            height: parent.height

            padding: 6

            backgroundColor: Material.backgroundColor

            scaleData: root.scaleData
            withText: false
        }
    }
}
