/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1

MyListItem {
    id: root

    Column {
        id: column

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }

        MyListItemText {
            width: parent.width
            mode: root.mode
            role: "SelectionTitle"
            text: name
        }

        MyListItemText {
            width: parent.width
            mode: root.mode
            role: "SelectionSubTitle"
            text: path
        }
    }
}
