/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import cuteTube 2.0
import QVimeo 1.0 as QVimeo
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    function load(videoOrId) {
        video.loadVideo(videoOrId);

        if (video.userId) {
            user.loadUser(video.userId);
        }
    }

    title: view.currentTab.title
    showProgressIndicator: (video.status == QVimeo.ResourcesRequest.Loading)
                           || (user.status == QVimeo.ResourcesRequest.Loading)
                           || (view.currentTab.showProgressIndicator)

    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolIcon {}
    }

    VimeoVideo {
        id: video

        onStatusChanged: {
            switch (status) {
            case QVimeo.ResourcesRequest.Ready:
                if (!user.id) {
                    user.loadUser(userId);
                }

                break;
            case QVimeo.ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    VimeoUser {
        id: user

        onStatusChanged: if (status == QVimeo.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    TabView {
        id: view

        anchors.fill: parent
        visible: video.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            title: qsTr("Video info")
            tools: ToolBarLayout {
                BackToolIcon {}

                MyToolIcon {
                    platformIconId: "toolbar-view-menu"
                    enabled: (video.id) && (user.id)
                    onClicked: menu.open()
                }
            }

            Menu {
                id: menu

                MenuLayout {

                    MenuItem {
                        text: qsTr("Download")
                        onClicked: {
                            dialogLoader.sourceComponent = downloadDialog;
                            dialogLoader.item.videoId = video.id;
                            dialogLoader.item.videoTitle = video.title;
                            dialogLoader.item.open();
                        }
                    }

                    MenuItem {
                        text: qsTr("Share")
                        onClicked: if (!ShareUi.shareVideo(video)) infoBanner.showMessage(qsTr("Unable to share video"));
                    }

                    MenuItem {
                        text: video.favourited ? qsTr("Unlike") : qsTr("Like")
                        enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.INTERACT_SCOPE))
                        onClicked: video.favourited ? video.unfavourite() : video.favourite()
                    }

                    MenuItem {
                        text: qsTr("Watch later")
                        enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.INTERACT_SCOPE))
                        onClicked: video.watchLater()
                    }

                    MenuItem {
                        text: qsTr("Add to album")
                        enabled: (Vimeo.userId) && (Vimeo.hasScope(Vimeo.EDIT_SCOPE))
                        onClicked: {
                            dialogLoader.sourceComponent = playlistDialog;
                            dialogLoader.item.load(video);
                            dialogLoader.item.open();
                        }
                    }
                }
            }

            VideoThumbnail {
                id: thumbnail

                z: 10
                width: parent.width - UI.PADDING_DOUBLE * 2
                height: Math.floor(width * 3 / 4)
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: UI.PADDING_DOUBLE
                }
                source: video.largeThumbnailUrl
                durationText: video.duration
                enabled: video.id ? true : false
                onClicked: {
                    if (Settings.videoPlayer == "cutetube") {
                        appWindow.pageStack.push(Qt.resolvedUrl("../VideoPlaybackPage.qml")).addVideos([video]);
                    }
                    else {
                        dialogLoader.sourceComponent = playbackDialog;
                        dialogLoader.item.model.list(video.id);
                        dialogLoader.item.open();
                    }
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
                        text: video.title
                    }

                    Avatar {
                        id: avatar

                        width: 60
                        height: 60
                        source: user.thumbnailUrl
                        onClicked: appWindow.pageStack.push(Qt.resolvedUrl("VimeoUserPage.qml")).load(user)
                    }

                    Item {
                        width: parent.width - avatar.width - UI.PADDING_DOUBLE
                        height: avatar.height

                        Label {
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                            }
                            verticalAlignment: Text.AlignTop
                            elide: Text.ElideRight
                            font.pixelSize: UI.FONT_SMALL
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
                            font.pixelSize: UI.FONT_SMALL
                            font.family: UI.FONT_FAMILY_LIGHT
                            text: qsTr("Published on") + " " + video.date
                        }
                    }                    

                    Label {
                        width: parent.width
                        text: video.description ? Utils.toRichText(video.description) : qsTr("No description")
                        onLinkActivated: {
                            var resource = Resources.getResourceFromUrl(link);

                            if (resource.service != Resources.VIMEO) {
                                Qt.openUrlExternally(link);
                                return;
                            }

                            if (resource.type == Resources.USER) {
                                appWindow.pageStack.push(Qt.resolvedUrl("VimeoUserPage.qml")).load(resource.id);
                            }
                            else if (resource.type == Resources.PLAYLIST) {
                                appWindow.pageStack.push(Qt.resolvedUrl("VimeoPlaylistPage.qml")).load(resource.id);
                            }
                            else {
                                appWindow.pageStack.push(Qt.resolvedUrl("VimeoVideoPage.qml")).load(resource.id);
                            }
                        }
                    }
                }
            }

            ScrollDecorator {
                flickableItem: flicker
            }

            Loader {
                id: dialogLoader
            }

            Component {
                id: playbackDialog

                VimeoPlaybackDialog {
                    onAccepted: VideoLauncher.playVideo(value.url)
                }
            }

            Component {
                id: downloadDialog

                VimeoDownloadDialog {}
            }

            Component {
                id: playlistDialog

                VimeoPlaylistDialog {}
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

        TabLoader {
            id: relatedTab

            width: view.width
            height: view.height
            title: qsTr("Related")
            tab: Component {
                VimeoVideosTab {
                    Component.onCompleted: model.list("/videos/" + video.id + "/videos",
                                                      {filter: "related", per_page: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: commentsTab

            width: view.width
            height: view.height
            title: qsTr("Comments")
            tab: Component {
                VimeoCommentsTab {
                    Component.onCompleted: model.list("/videos/" + video.id + "/comments", {per_page: MAX_RESULTS})
                }
            }
        }
    }
}
