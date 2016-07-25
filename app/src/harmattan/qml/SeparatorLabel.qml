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

import QtQuick 1.1
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Item {
    id: root

    property alias text: label.text

    height: 20

    Label {
        id: label

        anchors {
            right: parent.right
            rightMargin: UI.PADDING_XLARGE
            top: parent.top
        }
        font.pixelSize: UI.FONT_SMALL
        font.bold: true
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        height: 1
        anchors {
            left: parent.left
            right: label.left
            rightMargin: UI.PADDING_XXLARGE
            verticalCenter: label.verticalCenter
        }
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
    }
}
