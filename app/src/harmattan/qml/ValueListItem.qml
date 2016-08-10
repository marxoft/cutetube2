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

    property alias title: title.text
    property alias subTitle: subTitle.text

    Column {
        id: column

        anchors {
            left: parent.left
            leftMargin: UI.PADDING_DOUBLE
            right: icon.left
            rightMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: title

            width: parent.width
            font.bold: true
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }

        Label {
            id: subTitle

            width: parent.width
            font.pixelSize: UI.FONT_SMALL
            font.family: UI.FONT_FAMILY_LIGHT
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
            text: qsTr("None chosen")
        }
    }

    Image {
        id: icon

        anchors {
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
            verticalCenter: parent.verticalCenter
        }

        source: "image://theme/icon-m-textinput-combobox-arrow"
    }
}
