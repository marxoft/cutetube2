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


MyListItem {
    id: root

    signal thumbnailClicked

    height: thumbnail.height + platformStyle.paddingLarge * 2

    VideoThumbnail {
        id: thumbnail

        z: 100
        width: Math.floor(height * 4 / 3)
        height: platformStyle.graphicSizeLarge
        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }
        source: thumbnailUrl
        durationText: duration
        onClicked: root.thumbnailClicked()
    }

    Column {
        id: column

        anchors {
            left: thumbnail.right
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }

        MyListItemText {
            width: parent.width
            mode: root.mode
            role: "Title"
            text: title
        }

        MyListItemText {
            width: parent.width
            mode: root.mode
            role: "SubTitle"
            visible: username != ""
            text: qsTr("by") + " " + username
        }

        MyListItemText {
            width: parent.width
            mode: root.mode
            role: "SubTitle"
            visible: date != ""
            text: date
        }
    }

    onKeyPressed: {
        switch (key) {
        case Qt.Key_Space:
        case 112:
            thumbnailClicked();
            break;
        default:
            break;
        }
    }
}
