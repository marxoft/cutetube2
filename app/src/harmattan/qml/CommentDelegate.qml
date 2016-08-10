/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Item {
    id: root

    signal thumbnailClicked
    signal clicked
    signal pressAndHold

    x: UI.PADDING_DOUBLE
    width: ListView.view.width - UI.PADDING_DOUBLE * 2
    height: userLabel.height + commentLabel.height + 35

    BorderImage {
        id: background

        anchors.fill: parent
        source: mouseArea.pressed ? "images/comment-outgoing-pressed" + appStyle.__invertedString + ".png"
                                  : "images/comment-outgoing" + appStyle.__invertedString + ".png"
        border {
            left: 20
            top: 20
            right: 20
            bottom: 20
        }
    }

    Avatar {
        id: avatar

        z: 100
        width: 40
        height: 40
        anchors {
            top: parent.top
            topMargin: 15
            left: parent.left
            leftMargin: UI.PADDING_DOUBLE
        }
        source: thumbnailUrl === "" ? "images/avatar.png" : thumbnailUrl
        onClicked: root.thumbnailClicked()
    }

    Label {
        id: userLabel

        anchors {
            top: avatar.top
            left: avatar.right
            leftMargin: UI.PADDING_DOUBLE
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
        }
        font.pixelSize: UI.FONT_SMALL
        font.family: UI.FONT_FAMILY_LIGHT
        text: qsTr("by") + " " + username + " " + qsTr("on") + " " + date
    }

    Label {
        id: commentLabel

        anchors {
            top: userLabel.bottom
            left: userLabel.left
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
        }
        font.pixelSize: UI.FONT_SMALL
        text: "\"" + body + "\""
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onClicked: root.clicked()
        onPressAndHold: root.pressAndHold()
    }
}
