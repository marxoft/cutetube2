/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1

MyListItem {
    id: root

    Image {
        id: icon

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }

        source: parent.ListView.view.model.isFolder(index) ? "images/folder.png"
                                                           : "images/videofile.png"
    }

    MyListItemText {
        anchors {
            left: icon.right
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }
        mode: root.mode
        role: "Title"
        text: fileName
    }
}
