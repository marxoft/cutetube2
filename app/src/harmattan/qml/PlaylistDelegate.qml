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

ListItem {
    id: root

    height: thumbnail.height + UI.PADDING_DOUBLE * 2

    PlaylistThumbnail {
        id: thumbnail

        width: 100
        height: 75
        anchors {
            left: parent.left
            leftMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }
        source: thumbnailUrl
        text: videoCount == - 1 ? "" : videoCount > 0 ? videoCount + " " + qsTr("videos") : qsTr("No videos")
        enabled: false
    }

    Column {
        id: column

        anchors {
            left: thumbnail.right
            leftMargin: UI.PADDING_DOUBLE
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }

        Label {
            width: parent.width
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: title
        }

        Label {
            width: parent.width
            font.pixelSize: UI.FONT_SMALL
            font.family: UI.FONT_FAMILY_LIGHT
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            visible: (!username) && (!date) ? false : true
            text: (!username ? "" : qsTr("by") + " " + username + " ")
                  + (!date ? "" : (!username ? "" : qsTr("on") + " ") + date)
        }
    }
}
