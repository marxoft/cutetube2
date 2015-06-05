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

Rectangle {
    id: root
    
    property alias source: image.source
    property alias sourceSize: image.sourceSize
    property alias fillMode: image.fillMode
    property alias status: image.status
    property alias durationText: durationLabel.text
    
    signal clicked
    signal rightClicked
    
    color: "#000"
    border {
        width: 1
        color: palette.mid
    }
    
    Image {
        id: image
        
        anchors {
            fill: parent
            margins: 1
        }
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
    }
    
    Label {
        id: durationLabel
        
        anchors.centerIn: background
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        font.pixelSize: Math.floor(parent.height / 6)
        color: "#fff"
    }
    
    MouseArea {
        id: mouseArea
        
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: mouse.button == Qt.RightButton ? root.rightClicked() : root.clicked()
    } 
}
