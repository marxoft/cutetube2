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
import QtQuick.Controls 1.1

FocusScope {
    id: root
    
    property string title
    property alias tools: toolBar.children
    default property alias content: contentItem.data
    
    width: parent ? parent.width : undefined
    height: parent ? parent.height : undefined
    focus: true
    
    ToolBar {
        id: toolBar
        
        visible: children.length > 2
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }
    
    Item {
        id: contentItem
        
        anchors {
            left: parent.left
            right: parent.right
            top: toolBar.visible ? toolBar.bottom : parent.top
            bottom: parent.bottom
        }
        clip: true
    }
    
    onVisibleChanged: if (visible) forceActiveFocus();
    
    Component.onCompleted: forceActiveFocus()
}
