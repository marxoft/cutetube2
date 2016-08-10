/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Rectangle {
    id: root

    property alias source: image.source
    property alias sourceSize: image.sourceSize
    property alias fillMode: image.fillMode
    property alias status: image.status
    property alias text: label.text

    signal clicked

    color: "#000"
    border {
        width: 1
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
    }

    Image {
        id: image

        anchors {
            fill: parent
            margins: 1
        }
        opacity: mouseArea.pressed ? UI.OPACITY_DISABLED : UI.OPACITY_ENABLED
        fillMode: Image.PreserveAspectFit
        smooth: true
    }

    Rectangle {
        id: background

        height: Math.floor(parent.height / 4)
        anchors {
            left: image.left
            right: image.right
            bottom: image.bottom
        }
        color: "#000"
        opacity: 0.8
        visible: label.visible
    }

    Label {
        id: label

        anchors.centerIn: background
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Math.floor(parent.height / 6)
        color: "#fff"
        visible: text != ""
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
    }
}
