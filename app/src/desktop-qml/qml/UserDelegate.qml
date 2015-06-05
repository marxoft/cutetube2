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
import QtQuick.Controls 1.0

ItemDelegate {
    id: root

    height: avatar.height + 20
    
    Avatar {
        id: avatar
        
        z: 1
        width: 60
        height: 60
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        source: thumbnailUrl
        enabled: false
    }
    
    Column {
        id: column
        
        anchors {
            left: avatar.right
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: textLabel
            
            width: parent.width
            elide: Text.ElideRight
            color: root.GridView.isCurrentItem ? palette.light : palette.text
            text: username
        }

        Label {
            id: subtextLabel
            
            width: parent.width
            elide: Text.ElideRight
            color: root.GridView.isCurrentItem ? palette.light : palette.mid
            text: root.GridView.view.model.data(index, "subscriberCount") == undefined ? ""
                  : subscriberCount ? Utils.formatLargeNumber(subscriberCount) + " " + qsTr("subscribers")
                  : qsTr("No subscribers")
            visible: text != ""
        }
    }
    
    states: State {
        name: "grid"
        when: root.mode == "grid"
        
        PropertyChanges {
            target: root
            width: 110
            height: 140
        }
        
        PropertyChanges {
            target: avatar
            width: 90
            height: 90
        }
        
        AnchorChanges {
            target: column
            anchors {
                left: parent.left
                top: avatar.bottom
                verticalCenter: undefined
            }
        }
    }
}
