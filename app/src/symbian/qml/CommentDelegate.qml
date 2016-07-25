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
import com.nokia.symbian 1.1

Item {
    id: root

    signal thumbnailClicked
    signal clicked
    signal pressAndHold

    x: platformStyle.paddingLarge
    width: ListView.view.width - platformStyle.paddingLarge * 2
    height: userLabel.height + commentLabel.height + 35

    BorderImage {
        id: background

        anchors.fill: parent
        source: mouseArea.pressed ? "images/comment-outgoing-pressed.png"
                                  : "images/comment-outgoing.png"
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
        width: platformStyle.graphicSizeSmall
        height: platformStyle.graphicSizeSmall
        anchors {
            top: parent.top
            topMargin: 15
            left: parent.left
            leftMargin: platformStyle.paddingLarge
        }
        source: thumbnailUrl === "" ? "images/avatar.png" : thumbnailUrl
        onClicked: root.thumbnailClicked()
    }

    Label {
        id: userLabel

        anchors {
            top: avatar.top
            left: avatar.right
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
        }
        font.pixelSize: platformStyle.fontSizeSmall
        font.weight: Font.Light
        color: platformStyle.colorNormalMid
        wrapMode: Text.WordWrap
        text: qsTr("by") + " " + username + " " + qsTr("on") + " " + date
    }

    Label {
        id: commentLabel

        anchors {
            top: userLabel.bottom
            left: userLabel.left
            right: parent.right
            rightMargin: platformStyle.paddingLarge
        }
        font.pixelSize: platformStyle.fontSizeSmall
        wrapMode: Text.Wrap
        text: "\"" + body + "\""
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onClicked: root.clicked()
        onPressAndHold: root.pressAndHold()
    }
}
