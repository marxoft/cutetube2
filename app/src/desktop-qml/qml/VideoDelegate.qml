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

    height: thumbnail.height + 20
    
    VideoThumbnail {
        id: thumbnail
        
        z: 1
        width: 80
        height: 60
        anchors {
            left: parent.left
            top: parent.top
            margins: 10
        }
        source: thumbnailUrl
        durationText: duration
        enabled: false
    }
    
    Column {
        id: column
        
        anchors {
            left: thumbnail.right
            leftMargin: 10
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }

        Label {
            id: titleLabel
                      
            width: parent.width
            elide: Text.ElideRight
            color: root.GridView.isCurrentItem ? palette.light : palette.text
            text: title
        }

        Label {
            id: usernameLabel
              
            width: parent.width
            elide: Text.ElideRight
            color: root.GridView.isCurrentItem ? palette.light : palette.mid
            text: username ? username : ""
            visible: text != ""
        }
        
        Label {
            id: dateLabel
            
            width: parent.width
            elide: Text.ElideRight
            color: root.GridView.isCurrentItem ? palette.light : palette.mid
            text: date ? date : ""
            visible: text != ""
        }
    }
    
    states: State {
        name: "grid"
        when: root.mode == "grid"
        
        PropertyChanges {
            target: root
            width: 140
            height: 140
        }
        
        PropertyChanges {
            target: thumbnail
            width: 120
            height: 90
        }
        
        PropertyChanges {
            target: dateLabel
            visible: false
        }
        
        AnchorChanges {
            target: column
            anchors {
                left: parent.left
                top: thumbnail.bottom
                verticalCenter: undefined
            }
        }
    }
}
