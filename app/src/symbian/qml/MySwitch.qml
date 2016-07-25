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

    property alias text: title.text
    property alias checked: switcher.checked
    property alias pressed: switcher.pressed
    property bool platformInverted: false

    width: parent.width
    height: switcher.height + platformStyle.paddingLarge

    Keys.forwardTo: switcher

    Rectangle {
        x: platformStyle.paddingSmall
        y: platformStyle.paddingSmall
        width: parent.width - platformStyle.paddingSmall * 2
        height: parent.height - platformStyle.paddingSmall * 2
        visible: (root.activeFocus) && (symbian.listInteractionMode === Symbian.KeyNavigation)
        color: "transparent"
        radius: 5
        border {
            width: 1
            color: platformStyle.colorDisabledMid
        }
    }

    Label {
        id: title

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            right: switcher.left
            rightMargin: platformStyle.paddingSmall
            verticalCenter: parent.verticalCenter
        }

        color: root.enabled ? platformStyle.colorNormalLight : platformStyle.colorDisabledMid
        verticalAlignment: Text.AlignVCenter
        maximumLineCount: 2
        elide: Text.ElideRight
        wrapMode: Text.WordWrap
    }

    Switch {
        id: switcher

        anchors {
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }
    }
}
