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

    height: avatar.height + UI.PADDING_DOUBLE * 2

    Avatar {
        id: avatar

        width: 75
        height: 75
        anchors {
            left: parent.left
            leftMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }
        source: thumbnailUrl
        enabled: false
    }

    Column {
        id: column

        anchors {
            left: avatar.right
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
            text: username
        }

        Label {
            width: parent.width
            font.pixelSize: UI.FONT_SMALL
            font.family: UI.FONT_FAMILY_LIGHT
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: model["subscriberCount"] == undefined ? "" : Utils.formatLargeNumber(subscriberCount) + " " + qsTr("subscribers")
            visible: text != ""
        }
    }
}
