/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 2.0
import QtQuick.Controls 1.1

ItemDelegate {
    id: root
    

    signal thumbnailClicked

    height: userLabel.height + commentLabel.height + 30

    Avatar {
        id: avatar

        z: 100
        width: 40
        height: 40
        anchors {
            top: parent.top
            left: parent.left
            margins: 10
        }
        source: thumbnailUrl
        onClicked: root.thumbnailClicked()
    }

    Label {
        id: userLabel

        anchors {
            top: avatar.top
            left: avatar.right
            leftMargin: 10
            right: parent.right
            rightMargin: 10
        }
        wrapMode: Text.Wrap
        color: palette.mid
        text: qsTr("by") + " " + username + " " + qsTr("on") + " " + date
    }

    Label {
        id: commentLabel

        anchors {
            top: userLabel.bottom
            left: userLabel.left
            right: parent.right
            rightMargin: 10
        }
        wrapMode: Text.Wrap
        text: "\"" + body + "\""
    }
}
