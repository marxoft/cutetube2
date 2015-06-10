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
import QDailymotion 1.0 as QDailymotion
import ".."

Tab {
    id: root
        
    property variant model: item ? item.model : null
    property variant view: item ? item.view : null
    
    title: qsTr("Videos")
    
    FocusScope {
        id: content
        
        property alias model: videoModel
        property alias view: view
        
        width: parent ? parent.width : undefined
        height: parent ? parent.height : undefined
        focus: true
        
        DailymotionVideo {
            id: video
        }
    
        ItemView {
            id: view
        
            anchors {
                fill: parent
                leftMargin: mode == "grid" ? 10 : 0
            }
            mode: Settings.defaultViewMode
            model: DailymotionVideoModel {
                id: videoModel
                
                onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) messageBox.showError(errorString);
            }
            delegate: VideoDelegate {
                mode: view.mode
                onClicked: pageStack.push({item: Qt.resolvedUrl("DailymotionVideoPage.qml"), immediate: true})
                                         .load(videoModel.get(index))
                onRightClicked: contextMenu.popup()
            }
        }
    
        Menu {
            id: contextMenu
        
            MenuItem {
                text: qsTr("Download")
                iconName: "folder-download"
                onTriggered: {
                    loader.sourceComponent = downloadDialog;
                    loader.item.resourceId = videoModel.data(view.currentIndex, "id");
                    loader.item.resourceTitle = videoModel.data(view.currentIndex, "title");
                    loader.item.open();
                }
            }
            
            MenuItem {
                text: qsTr("Copy URL")
                iconName: "applications-internet"
                onTriggered: {
                    Clipboard.text = videoModel.data(view.currentIndex, "url");
                    statusBar.showMessage(qsTr("URL copied to clipboard"));
                }
            }
        
            MenuItem {
                text: videoModel.data(view.currentIndex, "favourited") ? qsTr("Unfavourite") : qsTr("Favourite")
                iconName: videoModel.data(view.currentIndex, "favourited") ? "list-remove" : "list-add"
                enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_FAVOURITES_SCOPE))
                onTriggered: {
                    video.loadVideo(videoModel.get(view.currentIndex));
                
                    if (video.favourited) {
                        video.unfavourite();
                    }
                    else {
                        video.favourite();
                    }
                }
            }
        
            MenuItem {
                text: qsTr("Add to playlist")
                iconName: "list-add"
                enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_PLAYLISTS_SCOPE))
                onTriggered: {
                    loader.sourceComponent = playlistDialog;
                    loader.item.load(videoModel.get(view.currentIndex));
                    loader.item.open();
                }
            }            
        }
        
        Loader {
            id: loader
        }
    
        Component {
            id: downloadDialog
        
            DailymotionDownloadDialog {}
        }
        
        Component {
            id: playlistDialog
            
            DailymotionPlaylistDialog {}
        }
    }    
}
