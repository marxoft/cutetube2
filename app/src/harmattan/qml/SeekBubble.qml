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

Image {

    property string value

    source: "images/seekbubble.png"
    opacity: 0

    Behavior on opacity { PropertyAnimation { properties: "opacity"; duration: 200 } }

    Label {
        anchors { fill: parent; bottomMargin: UI.PADDING_DOUBLE }
        text: parent.value
        font.pixelSize: text.length > 5 ? UI.FONT_SMALL : UI.FONT_SMALL
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
    }
}
