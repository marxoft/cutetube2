/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0

Rectangle {
    id: root

    property alias avatarSource: avatar.source
    property alias avatarSourceSize: avatar.sourceSize
    property alias avatarFillMode: avatar.fillMode
    property alias avatarStatus: avatar.status
    property alias bannerSource: banner.source
    property alias bannerSourceSize: banner.sourceSize
    property alias bannerFillMode: banner.fillMode
    property alias bannerStatus: banner.status

    signal clicked

    color: "#000"

    Image {
        id: banner

        anchors.fill: parent
        smooth: true

        Image {
            id: avatar

            width: height
            height: Math.floor(parent.height / 2)
            anchors {
                left: parent.left
                leftMargin: 10
            }
            smooth: true
        }
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
    }
}
