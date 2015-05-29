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

Button {
    id: root

    Rectangle {
        x: -platformStyle.paddingSmall
        y: -platformStyle.paddingSmall
        width: parent.width + platformStyle.paddingSmall * 2
        height: parent.height + platformStyle.paddingSmall * 2
        visible: (root.activeFocus) && (symbian.listInteractionMode === Symbian.KeyNavigation)
        color: "transparent"
        radius: 5
        border {
            width: 1
            color: platformStyle.colorDisabledMid
        }
    }
}
