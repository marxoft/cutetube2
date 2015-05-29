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

ToolButton {
    id: root

    property string toolTipText

    opacity: enabled ? 1 : 0.5

    ToolTip {
        id: toolTip

        target: root
        text: toolTipText
        visible: false
    }

    Timer {
        interval: 250
        running: (toolTipText) && (root.pressed)
        onTriggered: toolTip.visible = true
    }

    onPressedChanged: if (!pressed) toolTip.visible = false;
}
