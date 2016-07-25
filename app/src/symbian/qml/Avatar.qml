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
import cuteTube 2.0

MaskedItem {
    id: root

    property alias source: avatar.source
    property alias sourceSize: avatar.sourceSize
    property alias fillMode: avatar.fillMode
    property alias status: avatar.status

    signal clicked

    opacity: mouseArea.pressed ? 0.5 : 1
    mask: Image {
        width: root.width
        height: root.height
        source: "images/avatar-mask.png"
        fillMode: Image.Stretch
        smooth: true
    }

    Image {
        id: frame

        anchors.fill: parent
        source: "images/avatar-frame.png"
        sourceSize.width: width
        sourceSize.height: height
        smooth: true
        fillMode: Image.Stretch
        visible: avatar.status == Image.Ready

        Image {
            id: avatar

            z: -1
            anchors.fill: parent
            sourceSize.width: width
            sourceSize.height: height
            smooth: true
            fillMode: Image.PreserveAspectCrop
            clip: true
            onStatusChanged: if (status == Image.Error) source = "images/avatar.png";
        }
    }


    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
    }
}

