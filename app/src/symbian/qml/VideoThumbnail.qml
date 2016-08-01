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

Rectangle {
    id: root
    
    property alias source: image.source
    property alias sourceSize: image.sourceSize
    property alias fillMode: image.fillMode
    property alias status: image.status
    property alias durationText: durationLabel.text
    
    signal clicked
    
    color: "#000"
    border {
        width: 1
        color: platformStyle.colorNormalMid
    }
    
    Image {
        id: image
        
        anchors {
            fill: parent
            margins: 1
        }
        opacity: mouseArea.pressed ? 0.5 : 1
        fillMode: Image.PreserveAspectFit
        smooth: true
    }
    
    Rectangle {
        id: background
        
        width: durationLabel.width + 10
        height: Math.floor(parent.height / 4)
        anchors {
            right: image.right
            bottom: image.bottom
        }
        color: "#000"
        opacity: 0.8
        visible: durationLabel.visible
    }
    
    Label {
        id: durationLabel
        
        anchors.centerIn: background
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Math.floor(parent.height / 6)
        color: "#fff"
        visible: text != ""
    }
    
    MouseArea {
        id: mouseArea
        
        anchors.fill: parent
        enabled: root.enabled
        onClicked: root.clicked()
    }
}
