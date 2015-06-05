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
import cuteTube 2.0
import QDailymotion 1.0 as QDailymotion
import ".."

Page {
    id: root
    
    function load(playlistOrId) {
        playlist.loadPlaylist(playlistOrId);
        videoModel.list("/playlists/" + (playlist.id ? playlist.id : playlistOrId), {limit: 50});
    }
    
    title: qsTr("Playlist info")
    tools: ToolBarLayout {
        
        ToolButton {
            id: backButton
        
            text: qsTr("Back")
            tooltip: qsTr("Go back")
            iconName: "go-previous"
            visible: root.Stack.index > 0
            onClicked: {
                playerLoader.sourceComponent = undefined;
                pageStack.pop({immediate: true});
            }
        }
    
        Label {
            Layout.fillWidth: true
            text: root.title
        }
        
        ToolButton {
            id: reloadButton
    
            text: qsTr("Reload")
            tooltip: qsTr("Reload")
            iconName: "view-refresh"
            enabled: videoModel.status != QDailymotion.ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
    
        ViewModeButton {
            id: viewButton
        }
    }
    
    DailymotionPlaylist {
        id: playlist
    }
    
    DailymotionVideo {
        id: video

        onUserIdChanged: if (userId) user.loadUser(userId);
    }
    
    DailymotionUser {
        id: user
    }
    
    Loader {
        id: playerLoader
        
        height: Math.floor(width * 3 / 4)
        anchors {
            left: parent.left
            right: parent.horizontalCenter
            top: parent.top
            margins: 10
        }
        
        sourceComponent: placeholder
    }
    
    ScrollView {
        id: scrollView
        
        anchors {
            left: parent.left
            right: parent.horizontalCenter
            top: playerLoader.bottom
            topMargin: 10
            bottom: parent.bottom
        }
        
        Flickable {
            id: flicker
            
            anchors.fill: parent
            contentHeight: flow.height + 20
            
            Flow {
                id: flow
                
                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: 10
                }
                spacing: 10
                
                Label {
                    width: parent.width
                    wrapMode: Text.WordWrap
                    font.bold: true
                    font.pixelSize: 24
                    text: video.title
                }
                
                Avatar {
                    id: avatar

                    width: 60
                    height: 60
                    source: user.thumbnailUrl
                    onClicked: pageStack.push({item: Qt.resolvedUrl("DailymotionUserPage.qml"), immediate: true}).load(user)
                }

                Item {
                    width: parent.width - avatar.width - 10
                    height: avatar.height

                    Label {
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                        }
                        verticalAlignment: Text.AlignTop
                        elide: Text.ElideRight
                        text: user.username
                    }

                    Label {
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                        verticalAlignment: Text.AlignBottom
                        elide: Text.ElideRight
                        color: palette.mid
                        text: qsTr("Published on") + " " + video.date
                    }
                }
                
                Label {
                    width: parent.width
                    wrapMode: Text.Wrap
                    text: video.description ? Utils.toRichText(video.description) : qsTr("No description")
                    onLinkActivated: {
                        var resource = Resources.getResourceFromUrl(link);

                        if (resource.service != Resources.DAILYMOTION) {
                            Qt.openUrlExternally(link);
                            return;
                        }

                        if (resource.type == Resources.USER) {
                            pageStack.push(Qt.resolvedUrl("DailymotionUserPage.qml")).load(resource.id);
                        }
                        else if (resource.type == Resources.PLAYLIST) {
                            pageStack.push(Qt.resolvedUrl("DailymotionPlaylistPage.qml")).load(resource.id);
                        }
                        else {
                            pageStack.push(Qt.resolvedUrl("DailymotionVideoPage.qml")).load(resource.id);
                        }
                    }
                }
            }
        }
    }
    
    Menu {
        id: contextMenu
        
        MenuItem {
            text: qsTr("Download")
            iconName: "folder-download"
            onTriggered: {
                loader.sourceComponent = downloadDialog;
                loader.item.list(video.id, video.title);
                loader.item.open();
            }
        }
        
        MenuItem {
            text: qsTr("Copy URL")
            iconName: "applications-internet"
            onTriggered: {
                Clipboard.text = video.url;
                statusBar.showMessage(qsTr("URL copied to clipboard"));
            }
        }
        
        MenuItem {
            text: video.favourited ? qsTr("Unfavourite") : qsTr("Favourite")
            iconName: video.favourited ? "list-remove" : "list-add"
            enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_FAVOURITES_SCOPE))
            onTriggered: video.favourited ? video.unfavourite() : video.favourite()
        }
        
        MenuItem {
            text: qsTr("Add to playlist")
            iconName: "list-add"
            enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_PLAYLISTS_SCOPE))
            onTriggered: {
                loader.sourceComponent = playlistDialog;
                loader.item.load(video);
                loader.item.open();
            }
        }
    }        
    
    Item {
        anchors {
            left: parent.horizontalCenter
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    
        ItemView {
            id: videoView
        
            anchors {
                fill: parent
                leftMargin: mode == "grid" ? 10 : 0
            }
            mode: Settings.defaultViewMode
            model: DailymotionVideoModel {
                id: videoModel
                
                onStatusChanged: {
                    switch (status) {
                    case QDailymotion.ResourcesRequest.Ready:
                        if (canFetchMore) {
                            fetchMore();
                        }
                        
                        break;
                    case QDailymotion.ResourcesRequest.Failed:
                        messageBox.showError(errorString);
                        break;
                    default:
                        break;
                    }
                }
            }
            delegate: VideoDelegate {
                mode: videoView.mode
                onRightClicked: videoContexMenu.popup()
            }
            onCurrentIndexChanged: {
                if (currentIndex >= 0) {
                    var v = videoModel.get(currentIndex);
                    
                    if (v) {
                        video.loadVideo(v);
                    }
                }
            }
        }
    
        Menu {
            id: videoContexMenu
            
            MenuItem {
                text: qsTr("Play")
                onTriggered: {
                    playerLoader.sourceComponent = player;
                    playerLoader.item.stop();
                    playerLoader.item.playVideo(videoModel.data(videoView.currentIndex, "id"));
                }
            }
        
            MenuItem {
                text: qsTr("Download")
                iconName: "folder-download"
                onTriggered: {
                    loader.sourceComponent = downloadDialog;
                    loader.item.list(videoModel.data(videoView.currentIndex, "id"),
                                     videoModel.data(videoView.currentIndex, "title"));
                    loader.item.open();
                }
            }
        
            MenuItem {
                text: qsTr("Copy URL")
                iconName: "applications-internet"
                onTriggered: {
                    Clipboard.text = videoModel.data(videoView.currentIndex, "url");
                    statusBar.showMessage(qsTr("URL copied to clipboard"));
                }
            }
        
            MenuItem {
                text: videoModel.data(videoView.currentIndex, "favourited") ? qsTr("Unfavourite") : qsTr("Favourite")
                iconName: videoModel.data(videoView.currentIndex, "favourited") ? "list-remove" : "list-add"
                enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_FAVOURITES_SCOPE))
                onTriggered: {
                    video.loadVideo(videoModel.get(videoView.currentIndex));
                
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
                    loader.item.load(videoModel.get(videoView.currentIndex));
                    loader.item.open();
                }
            }
        }
    }
    
    Loader {
        id: loader
    }
    
    Component {
        id: playbackDialog
        
        DailymotionPlaybackDialog {}
    }

    Component {
        id: downloadDialog
        
        DailymotionDownloadDialog {}
    }
    
    Component {
        id: playlistDialog
        
        DailymotionPlaylistDialog {}
    }
    
    Component {
        id: placeholder
        
        VideoPlaceholder {            
            anchors.fill: parent
            source: video.largeThumbnailUrl
            durationText: video.duration
            onClicked: {
                playerLoader.sourceComponent = player;
                playerLoader.item.playVideo(video.id);
            }
            onRightClicked: contextMenu.popup()
        }
    }
    
    Component {
        id: player
        
        DailymotionVideoPlayer {            
            anchors.fill: parent
            
            Keys.forwardTo: videoView
        }
    }
    
    Stack.onStatusChanged: if (Stack.status == Stack.Inactive) playerLoader.sourceComponent = placeholder;
}
