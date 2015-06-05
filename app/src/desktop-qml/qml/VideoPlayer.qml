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
import QtQuick.Layouts 1.1
import QtMultimedia 5.0 as QtMultimedia

QtMultimedia.Video {
    id: player
    
    property int resumePosition: 0
    property bool maximised: false
    property alias formatText: formatLabel.text
    
    signal formatRequest
        
    function playUrl(url) {
        source = decodeURIComponent(url);
        play();
    }
        
    function toggleAspectRatio() {
        switch (fillMode) {
        case QtMultimedia.VideoOutput.PreserveAspectFit:
            fillMode = QtMultimedia.VideoOutput.PreserveAspectCrop;
            return;
        case QtMultimedia.VideoOutput.PreserveAspectCrop:
            fillMode = QtMultimedia.VideoOutput.Stretch;
            return;
        default:
            fillMode = QtMultimedia.VideoOutput.PreserveAspectFit;
            return;
        }
    }
    
    z: 1000
        
    Rectangle {
        z: -1
        anchors.fill: parent
        color: "#000"
    }
    
    MouseArea {
        id: mouseArea
        
        anchors.fill: parent
        hoverEnabled: true
        onDoubleClicked: maximised = !maximised
    }
    
    Item {
        id: controls
    
        z: 100
        height: 36
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
        visible: mouseArea.containsMouse
        
        Rectangle {        
            anchors.fill: parent
            color: "#000"
            opacity: 0.8
        }
        
        Item {
            id: progressBar
            
            height: 6
            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }
        
            Rectangle {            
                height: 6
                width: player.duration > 0 ? Math.floor((player.position / player.duration) * parent.width) : 0
                anchors {
                    left: parent.left
                    top: parent.top
                }
                color: palette.highlight
            }
            
            MouseArea {
                anchors.fill: parent
                enabled: player.duration > 0
                onClicked: player.seek(Math.floor((mouseX / width) * player.duration))
            }
        }
        
        RowLayout {
            id: row
            
            height: 30
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            
            VideoPlayerButton {
                id: playButton
                
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                source: player.playbackState == QtMultimedia.MediaPlayer.PlayingState ? "images/pause.png"
                                                                                      : "images/play.png"
                onClicked: player.playbackState == QtMultimedia.MediaPlayer.PlayingState ? player.pause()
                                                                                         : player.play()
            }

            Label {
                id: positionLabel
                
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#fff"
                text: Utils.formatMSecs(player.position) + " / " + Utils.formatMSecs(player.duration)
            }
            
            Item {
                Layout.fillWidth: true
            }
            
            Label {
                id: formatLabel
    
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                color: "#fff"
        
                MouseArea {
                    anchors.fill: parent
                    onClicked: player.formatRequest()
                }
            }
            
            VideoPlayerButton {
                id: maximiseButton
                
                Layout.maximumWidth: 30
                Layout.maximumHeight: 30
                source: "images/fullscreen.png"
                onClicked: player.maximised = !player.maximised
            }
        }
    }
    
    Loader {
        id: loader
    }
    
    StateGroup {
        states: State {
            name: "maximised"
            when: player.maximised
                
            PropertyChanges {
                target: windowStates
                state: "playerMaximised"
            }
        
            ParentChange {
                target: player
                parent: window.contentItem
            }
        }        
    }
    
    Keys.onPressed: {
        switch (event.key) {
        case Qt.Key_P:
        case Qt.Key_Space:
            playbackState == QtMultimedia.MediaPlayer.PlayingState ? pause() : play();
            break;
        case Qt.Key_S:
            stop();
            break;
        case Qt.Key_A:
            toggleAspectRatio();
            break;
        case Qt.Key_F:
            maximised = !maximised;
            break;
        default:
            break;
        }
    }
    
    onErrorChanged: {
        if ((error !== QtMultimedia.MediaPlayer.NoError) && (source != "")) {
            messageBox.showError(qsTr("Cannot play video"));
        }
    }
    onStatusChanged: {
        switch (status) {
        case QtMultimedia.MediaPlayer.Buffered:
            if (resumePosition > 0) {
                seek(resumePosition);
                resumePosition = 0;
            }

            break;
        default:
            break;
        }
    }
    onParentChanged: forceActiveFocus()
    
    Component.onCompleted: forceActiveFocus()
}
