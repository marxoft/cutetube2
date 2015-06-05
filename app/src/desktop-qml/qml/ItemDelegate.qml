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

import QtQuick 2.0

Item {
    id: root
    
    property string mode: "list"
    default property alias content: contentItem.data
    
    signal clicked
    signal rightClicked
    
    width: parent ? parent.width : undefined
    height: 30
    
    Item {
        id: contentItem
        
        z: 10
        anchors.fill: parent
    }
    
    MouseArea {
        id: mouseArea
        
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onPressed: {
            if (root.GridView.view) {
                root.GridView.view.currentIndex = index;
            }
            else {
                root.ListView.view.currentIndex = index;
            }
        }
        onClicked: mouse.button == Qt.RightButton ? root.rightClicked() : root.clicked()
    }    
}
