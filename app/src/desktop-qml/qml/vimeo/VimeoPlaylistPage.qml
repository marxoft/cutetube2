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
import QVimeo 1.0 as QVimeo
import ".."

Page {
    id: root
    
    function load(playlistOrId) {
        playlist.loadPlaylist(playlistOrId);
    }
    
    title: qsTr("Album info")
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
            enabled: videoModel.status != QVimeo.ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
    
        ViewModeButton {
            id: viewButton
        }
    }
    
    VimeoPlaylist {
        id: playlist
        
        onUserIdChanged: if (userId) videosModel.list("/users/" + userId + "/albums/" + id + "/videos", {per_page: 50});
    }
    
    VimeoVideo {
        id: video

        onUserIdChanged: if (userId) user.loadUser(userId);
    }
    
    VimeoUser {
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
                    onClicked: pageStack.push({item: Qt.resolvedUrl("VimeoUserPage.qml"), immediate: true}).load(user)
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

                        if (resource.service != Resources.VIMEO) {
                            Qt.openUrlExternally(link);
                            return;
                        }

                        if (resource.type == Resources.USER) {
                            pageStack.push(Qt.resolvedUrl("VimeoUserPage.qml")).load(resource.id);
                        }
                        else if (resource.type == Resources.PLAYLIST) {
                            pageStack.push(Qt.resolvedUrl("VimeoPlaylistPage.qml")).load(resource.id);
                        }
                        else {
                            pageStack.push(Qt.resolvedUrl("VimeoVideoPage.qml")).load(resource.id);
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
                loader.item.resourceId = video.id;
                loader.item.resourceTitle = video.title;
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
            text: video.favourited ? qsTr("Unlike") : qsTr("Like")
            iconName: video.favourited ? "list-remove" : "list-add"
            enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.INTERACT_SCOPE))
            onTriggered: video.favourited ? video.unfavourite() : video.favourite()
        }
        
        MenuItem {
            text: qsTr("Watch later")
            iconName: "bookmark-new"
            enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.INTERACT_SCOPE))
            onTriggered: video.watchLater()
        }
        
        MenuItem {
            text: qsTr("Add to album")
            iconName: "list-add"
            enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.EDIT_SCOPE))
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
            model: VimeoVideoModel {
                id: videoModel
                
                onStatusChanged: {
                    switch (status) {
                    case QVimeo.ResourcesRequest.Ready:
                        if (canFetchMore) {
                            fetchMore();
                        }
                        
                        break;
                    case QVimeo.ResourcesRequest.Failed:
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
                    loader.item.resourceId = videoModel.data(videoView.currentIndex, "id");
                    loader.item.resourceTitle = videoModel.data(videoView.currentIndex, "title");
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
                text: videoModel.data(videoView.currentIndex, "favourited") ? qsTr("Unlike") : qsTr("Like")
                iconName: videoModel.data(videoView.currentIndex, "favourited") ? "list-remove" : "list-add"
                enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.INTERACT_SCOPE))
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
                text: qsTr("Watch later")
                iconName: "bookmark-new"
                enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.INTERACT_SCOPE))
                onTriggered: {
                    video.loadVideo(videoModel.get(videoView.currentIndex));
                    video.watchLater();
                }
            }
        
            MenuItem {
                text: qsTr("Add to album")
                iconName: "list-add"
                enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.EDIT_SCOPE))
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
        
        VimeoPlaybackDialog {}
    }

    Component {
        id: downloadDialog
        
        VimeoDownloadDialog {}
    }
    
    Component {
        id: playlistDialog
        
        VimeoPlaylistDialog {}
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
        
        VimeoVideoPlayer {            
            anchors.fill: parent
            onRightClicked: contextMenu.popup()
            
            Keys.forwardTo: videoView
        }
    }
    
    Stack.onStatusChanged: if (Stack.status == Stack.Inactive) playerLoader.sourceComponent = placeholder;
}
