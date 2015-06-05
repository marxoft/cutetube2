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
import cuteTube 2.0
import QVimeo 1.0 as QVimeo
import ".."

VideoPlayer {
    id: player
    
    function playVideo(id) {
        streamModel.list(id);
    }
    
    VimeoStreamModel {
        id: streamModel

        onStatusChanged: {
            switch (status) {
            case QVimeo.StreamsRequest.Ready:
                if (count > 0) {
                    var i = match("name", Settings.defaultPlaybackFormat(Resources.VIMEO));

                    if ((loader.sourceComponent == streamDialog) && (loader.item.visible)) {
                        loader.item.currentIndex = Math.max(0, i);
                    }
                    else if (i >= 0) {
                        var v = data(i, "value");
                        player.playUrl(v.url);
                        formatText = v.height + "p";
                    }
                    else {
                        loader.sourceComponent = streamDialog;
                        loader.item.open();
                    }
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case QVimeo.StreamsRequest.Failed:
                messageBox.showError(errorString);
                break;
            default:
                break;
            }
        }
    }
    
    Loader {
        id: loader
    }
    
    Component {
        id: streamDialog
        
        MyDialog {
            property alias currentIndex: streamSelector.currentIndex
            
            minimumWidth: 400
            minimumHeight: streamSelector.height + 60
            title: qsTr("Video format")
            content: ComboBox {
                id: streamSelector
                
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }
                model: streamModel
                textRole: "name"
            }
            buttons: [
                Button {
                    text: qsTr("&Cancel")
                    iconName: "dialog-cancel"
                    onClicked: reject()
                },
        
                Button {
                    text: qsTr("&Ok")
                    iconName: "dialog-ok"
                    isDefault: true
                    enabled: (streamModel.status == QVimeo.ResourcesRequest.Ready) && (streamModel.count > 0)
                    onClicked: accept()
                }
            ]
            
            onRejected: streamModel.cancel()
            onAccepted: {
                Settings.setDefaultPlaybackFormat(Resources.VIMEO, streamModel.data(streamSelector.currentIndex, "name"));
                var value = streamModel.data(streamSelector.currentIndex, "value");
                player.playUrl(value.url);
                formatText = value.height + "p";
            }
        }
    }
    
    onFormatRequest: {
        resumePosition = position;
        pause();
        
        loader.sourceComponent = streamDialog;
        loader.item.open();
        streamModel.reload();
    }
}
