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
import com.nokia.symbian 1.1
import cuteTube 2.0
import QDailymotion 1.0 as QDailymotion
import ".."


MyPage {
    id: root

    function load(videoOrId) {
        video.loadVideo(videoOrId);

        if (video.userId) {
            user.loadUser(video.userId);
        }
    }

    title: view.currentTab.title
    showProgressIndicator: (video.status == QDailymotion.ResourcesRequest.Loading)
                           || (user.status == QDailymotion.ResourcesRequest.Loading)
                           || (view.currentTab.showProgressIndicator)

    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolButton {}
    }

    DailymotionVideo {
        id: video

        onStatusChanged: {
            switch (status) {
            case QDailymotion.ResourcesRequest.Ready:
                if (!user.id) {
                    user.loadUser(userId);
                }

                break;
            case QDailymotion.ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    DailymotionUser {
        id: user

        onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
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
                BackToolButton {}

                MyToolButton {
                    iconSource: "toolbar-view-menu"
                    toolTipText: qsTr("Options")
                    enabled: (video.id) && (user.id)
                    onClicked: menu.open()
                }
            }

            MyMenu {
                id: menu

                focusItem: flicker

                MenuLayout {

                    MenuItem {
                        text: qsTr("Download")
                        onClicked: {
                            dialogLoader.sourceComponent = downloadDialog;
                            dialogLoader.item.list(video.id, video.title);
                            dialogLoader.item.open();
                        }
                    }

                    MenuItem {
                        text: qsTr("Copy URL")
                        onClicked: {
                            Clipboard.text = video.url;
                            infoBanner.showMessage(qsTr("URL copied to clipboard"));
                        }
                    }

                    MenuItem {
                        text: video.favourited ? qsTr("Unfavourite") : qsTr("Favourite")
                        enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_FAVOURITES_SCOPE))
                        onClicked: video.favourited ? video.unfavourite() : video.favourite()
                    }

                    MenuItem {
                        text: qsTr("Add to playlist")
                        enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_PLAYLISTS_SCOPE))
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
                width: parent.width - platformStyle.paddingLarge * 2
                height: Math.floor(width * 3 / 4)
                anchors {
                    left: parent.left
                    top: parent.top
                    margins: platformStyle.paddingLarge
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

            MyFlickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: thumbnail.bottom
                    topMargin: platformStyle.paddingLarge
                    bottom: parent.bottom
                }
                contentHeight: flow.height + platformStyle.paddingLarge * 2
                clip: true

                Flow {
                    id: flow

                    anchors {
                        left: parent.left
                        leftMargin: platformStyle.paddingLarge
                        right: parent.right
                        rightMargin: platformStyle.paddingLarge
                        top: parent.top
                    }
                    spacing: platformStyle.paddingLarge

                    Label {
                        width: parent.width
                        font.bold: true
                        wrapMode: Text.WordWrap
                        text: video.title
                    }

                    Avatar {
                        id: avatar

                        width: platformStyle.graphicSizeMedium
                        height: platformStyle.graphicSizeMedium
                        source: user.thumbnailUrl
                        onClicked: appWindow.pageStack.push(Qt.resolvedUrl("DailymotionUserPage.qml")).load(user)
                    }

                    Item {
                        width: parent.width - avatar.width - platformStyle.paddingLarge
                        height: avatar.height

                        Label {
                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                            }
                            verticalAlignment: Text.AlignTop
                            elide: Text.ElideRight
                            font.pixelSize: platformStyle.fontSizeSmall
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
                            font.pixelSize: platformStyle.fontSizeSmall
                            font.weight: Font.Light
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
                                appWindow.pageStack.push(Qt.resolvedUrl("DailymotionUserPage.qml")).load(resource.id);
                            }
                            else if (resource.type == Resources.PLAYLIST) {
                                appWindow.pageStack.push(Qt.resolvedUrl("DailymotionPlaylistPage.qml")).load(resource.id);
                            }
                            else {
                                appWindow.pageStack.push(Qt.resolvedUrl("DailymotionVideoPage.qml")).load(resource.id);
                            }
                        }
                    }
                }
            }

            MyScrollBar {
                flickableItem: flicker
            }

            Loader {
                id: dialogLoader
            }

            Component {
                id: playbackDialog

                DailymotionPlaybackDialog {
                    focusItem: flicker
                    onAccepted: VideoLauncher.playVideo(value.url)
                }
            }

            Component {
                id: downloadDialog

                DailymotionDownloadDialog {
                    focusItem: flicker
                }
            }

            Component {
                id: playlistDialog

                DailymotionPlaylistDialog {
                    focusItem: flicker
                }
            }

            states: State {
                name: "landscape"
                when: !appWindow.inPortrait

                PropertyChanges {
                    target: thumbnail
                    width: 280
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
                DailymotionVideosTab {
                    Component.onCompleted: model.list("/video/" + video.id + "/related",
                                                      {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: commentsTab

            width: view.width
            height: view.height
            title: qsTr("Comments")
            tab: Component {
                DailymotionCommentsTab {
                    Component.onCompleted: model.list("/video/" + video.id + "/comments",
                                                      {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                }
            }
        }
    }
}
