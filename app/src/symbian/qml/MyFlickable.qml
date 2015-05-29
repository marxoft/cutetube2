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

Flickable {
    property ScrollBar scrollIndicator: null

    function moveUp() {
        if (scrollIndicator) {
            scrollIndicator.flash();
        }

        if (contentY > 100) {
            contentY -= 100;
        }
        else {
            contentY = 0;
        }
    }

    function moveDown() {
        if (scrollIndicator) {
            scrollIndicator.flash();
        }

        if (contentY < contentHeight - height - 100) {
            contentY += 100;
        }
        else {
            contentY = contentHeight - height;
        }
    }

    Keys.onUpPressed: moveUp()
    Keys.onDownPressed: moveDown()

    Component.onCompleted: forceActiveFocus()
    onVisibleChanged: if (visible) forceActiveFocus();

    Connections {
        target: inputContext
        onVisibleChanged: if ((!inputContext.visible) && (visible)) forceActiveFocus();
    }

    Behavior on contentY { NumberAnimation { duration: 250 } }
}
