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
import ".."

Page {
    id: root
    
    function load(playlistOrId) {
        if (playlistOrId.hasOwnProperty("id")) {
            playlist.loadPlaylist(playlistOrId);
            videoModel.list(playlist.id);
        }
        else {
            playlist.loadPlaylist(Settings.currentService, playlistOrId)
            videoModel.list(playlistOrId);
        }
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
            enabled: videoModel.status != ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
    
        ViewModeButton {
            id: viewButton
        }
    }
    
    PluginPlaylist {
        id: playlist
    }
    
    PluginVideo {
        id: video

        onUserIdChanged: if (userId) user.loadUser(service, userId);
    }
    
    PluginUser {
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
                    visible: user.id != ""
                    onClicked: pageStack.push({item: Qt.resolvedUrl("PluginUserPage.qml"), immediate: true}).load(user)
                }

                Item {
                    width: parent.width - avatar.width - 10
                    height: avatar.visible ? avatar.height : dateLabel.height

                    Label {
                        id: userLabel
                        
                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                        }
                        verticalAlignment: Text.AlignTop
                        elide: Text.ElideRight
                        text: user.username
                        visible: avatar.visible
                    }

                    Label {
                        id: dateLabel
                        
                        anchors {
                            left: parent.left
                            right: parent.right
                            bottom: parent.bottom
                        }
                        verticalAlignment: Text.AlignBottom
                        elide: Text.ElideRight
                        color: palette.mid
                        text: qsTr("Published on") + " " + (video.date ? video.date : qsTr("date unknown"))
                    }
                }
                
                Label {
                    width: parent.width
                    wrapMode: Text.Wrap
                    text: video.description ? Utils.toRichText(video.description) : qsTr("No description")
                    onLinkActivated: {
                        var resource = Resources.getResourceFromUrl(link);

                        if (resource.service != Settings.currentService) {
                            Qt.openUrlExternally(link);
                            return;
                        }

                        if (resource.type == Resources.USER) {
                            pageStack.push(Qt.resolvedUrl("PluginUserPage.qml")).load(resource.id);
                        }
                        else if (resource.type == Resources.PLAYLIST) {
                            pageStack.push(Qt.resolvedUrl("PluginPlaylistPage.qml")).load(resource.id);
                        }
                        else {
                            pageStack.push(Qt.resolvedUrl("PluginVideoPage.qml")).load(resource.id);
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
            enabled: video.downloadable
            onTriggered: {
                loader.sourceComponent = downloadDialog;
                loader.item.resourceId = video.id;
                loader.item.resourceTitle = video.title;
                loader.item.streamUrl = video.streamUrl;
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
            model: PluginVideoModel {
                id: videoModel
                
                service: Settings.currentService
                onStatusChanged: {
                    switch (status) {
                    case ResourcesRequest.Ready:
                        if (canFetchMore) {
                            fetchMore();
                        }
                        
                        break;
                    case ResourcesRequest.Failed:
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
                    
                    var streamUrl = videoModel.data(videoView.currentIndex, "streamUrl").toString();
                
                    if (streamUrl) {
                        playerLoader.item.playUrl(streamUrl);
                    }
                    else {
                        playerLoader.item.playVideo(videoModel.data(videoView.currentIndex, "id"));
                    }
                }
            }
        
            MenuItem {
                text: qsTr("Download")
                iconName: "folder-download"
                enabled: (videoView.currentIndex >= 0) && (videoModel.data(videoView.currentIndex, "downloadable"))
                onTriggered: {
                    loader.sourceComponent = downloadDialog;
                    loader.item.resourceId = videoModel.data(videoView.currentIndex, "id");
                    loader.item.resourceTitle = videoModel.data(videoView.currentIndex, "title");
                    loader.item.streamUrl = videoModel.data(videoView.currentIndex, "streamUrl");
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
        }
    }
    
    Loader {
        id: loader
    }
    
    Component {
        id: playbackDialog
        
        PluginPlaybackDialog {}
    }

    Component {
        id: downloadDialog
        
        PluginDownloadDialog {}
    }
    
    Component {
        id: placeholder
        
        VideoPlaceholder {            
            anchors.fill: parent
            source: video.largeThumbnailUrl
            durationText: video.duration
            onClicked: {
                playerLoader.sourceComponent = player;
                var streamUrl = video.streamUrl.toString();
                
                if (streamUrl) {
                    playerLoader.item.playUrl(streamUrl);
                }
                else {
                    playerLoader.item.playVideo(video.id);
                }
            }
            onRightClicked: contextMenu.popup()
        }
    }
    
    Component {
        id: player
        
        PluginVideoPlayer {            
            anchors.fill: parent
            onRightClicked: contextMenu.popup()
            
            Keys.forwardTo: videoView
        }
    }
    
    Stack.onStatusChanged: if (Stack.status == Stack.Inactive) playerLoader.sourceComponent = placeholder;
}
