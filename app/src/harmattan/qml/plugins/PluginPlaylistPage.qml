/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import cuteTube 2.0
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    function load(playlistOrId) {
        if (playlistOrId.hasOwnProperty("id")) {
            playlist.loadPlaylist(playlistOrId);
            videosTab.model.list(playlist.id);

            if (playlist.userId) {
                user.loadUser(playlist.service, playlist.userId);
            }
        }
        else {
            playlist.loadPlaylist(Settings.currentService, playlistOrId)
            videosTab.model.list(playlistOrId);
        }
    }

    title: view.currentTab.title
    showProgressIndicator: (playlist.status == ResourcesRequest.Loading)
                           || (user.status == ResourcesRequest.Loading)
                           || (videosTab.model.status == ResourcesRequest.Loading)
    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolIcon {}
    }

    PluginPlaylist {
        id: playlist

        onStatusChanged: {
            switch (status) {
            case ResourcesRequest.Ready:
                if ((userId) && (userId != user.id)) {
                    user.loadUser(service, userId);
                }

                break;
            case ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    PluginUser {
        id: user

        onStatusChanged: if (status == ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    TabView {
        id: view

        anchors.fill: parent
        visible: playlist.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            title: qsTr("Playlist info")

            PlaylistThumbnail {
                id: thumbnail

                z: 10
                width: parent.width - UI.PADDING_DOUBLE * 2
                height: Math.floor(width * 3 / 4)
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: UI.PADDING_DOUBLE
                }
                source: playlist.largeThumbnailUrl
                text: playlist.videoCount ? playlist.videoCount + " " + qsTr("videos") : qsTr("No videos")
                enabled: (videosTab.model.count > 0) && (Settings.videoPlayer == "cutetube")
                onClicked: {
                    var videos = [];

                    for (var i = 0; i < videosTab.model.count; i ++) {
                        videos.push(videosTab.model.get(i));
                    }

                    appWindow.pageStack.push(Qt.resolvedUrl("../VideoPlaybackPage.qml")).addVideos(videos);
                }
            }

            Flickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: thumbnail.bottom
                    topMargin: UI.PADDING_DOUBLE
                    bottom: parent.bottom
                }
                contentHeight: flow.height + UI.PADDING_DOUBLE * 2
                clip: true

                Flow {
                    id: flow

                    anchors {
                        left: parent.left
                        leftMargin: UI.PADDING_DOUBLE
                        right: parent.right
                        rightMargin: UI.PADDING_DOUBLE
                        top: parent.top
                    }
                    spacing: UI.PADDING_DOUBLE

                    Label {
                        width: parent.width
                        font.bold: true
                        wrapMode: Text.WordWrap
                        text: playlist.title
                    }

                    Avatar {
                        id: avatar

                        width: 60
                        height: 60
                        source: user.thumbnailUrl
                        visible: user.thumbnailUrl.toString() != ""
                        onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginUserPage.qml")).load(user)
                    }

                    Item {
                        width: parent.width - avatar.width - UI.PADDING_DOUBLE
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
                            font.pixelSize: UI.FONT_SMALL
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
                            font.pixelSize: UI.FONT_SMALL
                            font.family: UI.FONT_FAMILY_LIGHT
                            text: qsTr("Published on") + " " + playlist.date
                            visible: playlist.date != ""
                        }
                    }

                    Label {
                        width: parent.width
                        text: playlist.description ? Utils.toRichText(playlist.description) : qsTr("No description")
                        onLinkActivated: {
                            var resource = Resources.getResourceFromUrl(link);

                            if (resource.service != Settings.currentService) {
                                Qt.openUrlExternally(link);
                                return;
                            }

                            if (resource.type == Resources.USER) {
                                appWindow.pageStack.push(Qt.resolvedUrl("PluginUserPage.qml")).load(resource.id);
                            }
                            else if (resource.type == Resources.PLAYLIST) {
                                appWindow.pageStack.push(Qt.resolvedUrl("PluginPlaylistPage.qml")).load(resource.id);
                            }
                            else {
                                appWindow.pageStack.push(Qt.resolvedUrl("PluginVideoPage.qml")).load(resource.id);
                            }
                        }
                    }
                }
            }

            ScrollDecorator {
                flickableItem: flicker
            }

            states: State {
                name: "landscape"
                when: !appWindow.inPortrait

                PropertyChanges {
                    target: thumbnail
                    width: 320
                }

                AnchorChanges {
                    target: flicker
                    anchors {
                        left: thumbnail.right
                        top: parent.top
                    }
                }
            }
        }

        PluginVideosTab {
            id: videosTab

            width: view.width
            height: view.height
            title: qsTr("Videos")
        }
    }

    Connections {
        target: videosTab.model
        onStatusChanged: {
            if ((videosTab.model.status == ResourcesRequest.Ready) && (videosTab.model.canFetchMore)) {
                videosTab.model.fetchMore();
            }
        }
    }
}
