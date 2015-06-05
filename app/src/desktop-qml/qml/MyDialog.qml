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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.1

Window {
    id: root
    
    property alias content: contentItem.data
    property alias buttons: buttonRow.data
    
    signal opened
    signal closed
    signal accepted
    signal rejected
    
    function open() {
        x = Math.floor(window.x + ((window.width - width) / 2));
        y = Math.floor(window.y + ((window.height - height) / 2));
        show();
        opened();
    }
    
    function accept() {
        hide();
        closed();
        accepted();
    }
    
    function reject() {
        hide();
        closed();
        rejected();
    }
    
    width: minimumWidth
    height: minimumHeight
    color: palette.window
    
    Item {
        id: contentItem
        
        anchors {
            left: parent.left
            top: parent.top
            right: parent.right
            bottom: buttonRow.visible ? buttonRow.top : parent.bottom
            margins: 10
        }
    }
    
    RowLayout {
        id: buttonRow
        
        anchors {
            right: parent.right
            bottom: parent.bottom
            margins: 10
        }
        
        visible: children.length > 0
    }
}
