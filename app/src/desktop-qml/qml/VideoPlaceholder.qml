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
import QtQuick.Layouts 1.0

Rectangle {
    id: root
    
    property alias source: image.source
    property alias sourceSize: image.sourceSize
    property alias fillMode: image.fillMode
    property alias status: image.status
    property string durationText
    
    signal clicked
    signal rightClicked
    
    color: "#000"
    
    Image {
        id: image
        
        anchors.fill: parent
        fillMode: Image.PreserveAspectFit
        smooth: true
    }
    
    Item {
        id: controls
    
        z: 1
        height: 30
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        
        Rectangle {        
            anchors.fill: parent
            color: "#000"
            opacity: 0.8
        }
        
        RowLayout {
            id: row
            
            anchors.fill: parent
            
            Image {
                id: playButton
                
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                source: "images/play.png"
            }
            
            Label {
                id: durationLabel
                
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#fff"
                text: root.durationText ? "00:00 / " + root.durationText : "--:-- / --:--"
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Image {
                id: maximiseButton
                
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                source: "images/fullscreen.png"
            }
        }
    }
    
    MouseArea {
        id: mouseArea
        
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton | Qt.RightButton
        onClicked: mouse.button == Qt.RightButton ? root.rightClicked() : root.clicked()
    } 
}
