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
import com.nokia.symbian 1.1

Item {
    id: root

    property alias text: label.text

    width: !parent ? implicitWidth : parent.width
    height: platformStyle.graphicSizeLarge

    Label {
        id: label

        anchors {
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            top: parent.top
        }
        font.pixelSize: platformStyle.fontSizeSmall
        font.bold: true
        color: platformStyle.colorNormalMid
        horizontalAlignment: Text.AlignRight
        verticalAlignment: Text.AlignVCenter
    }

    Rectangle {
        height: 1
        anchors {
            left: parent.left
            right: label.left
            rightMargin: platformStyle.paddingLarge * 2
            verticalCenter: label.verticalCenter
        }
        color: platformStyle.colorNormalMid
    }
}
