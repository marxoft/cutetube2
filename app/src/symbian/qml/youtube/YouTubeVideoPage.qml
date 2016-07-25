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
import com.nokia.symbian 1.1
import cuteTube 2.0
import QYouTube 1.0 as QYouTube
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
    showProgressIndicator: (video.status == QYouTube.ResourcesRequest.Loading)
                           || (user.status == QYouTube.ResourcesRequest.Loading)
                           || (view.currentTab.showProgressIndicator)

    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolButton {}
    }

    YouTubeVideo {
        id: video

        property bool hasWriteScope: (YouTube.userId) && ((YouTube.hasScope(YouTube.READ_WRITE_SCOPE))
                                                          || (YouTube.hasScope(YouTube.FORCE_SSL_SCOPE)))

        onStatusChanged: {
            switch (status) {
            case QYouTube.ResourcesRequest.Ready:
                if (!user.id) {
                    user.loadUser(userId);
                }

                break;
            case QYouTube.ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    YouTubeUser {
        id: user

        onStatusChanged: if (status == QYouTube.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
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
                            dialogLoader.item.resourceId = video.id;
                            dialogLoader.item.resourceTitle = video.title;
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
                        enabled: video.hasWriteScope
                        onClicked: video.favourited ? video.unfavourite() : video.favourite()
                    }

                    MenuItem {
                        text: qsTr("Watch later")
                        enabled: video.hasWriteScope
                        onClicked: video.watchLater()
                    }

                    MenuItem {
                        text: qsTr("Add to playlist")
                        enabled: video.hasWriteScope
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
                        onClicked: appWindow.pageStack.push(Qt.resolvedUrl("YouTubeUserPage.qml")).load(user)
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
                            color: platformStyle.colorNormalMid
                            text: qsTr("Published on") + " " + video.date
                        }
                    }

                    MyButton {
                        id: likeButton

                        width: height
                        iconSource: video.liked ? Qt.resolvedUrl("../images/like-" + ACTIVE_COLOR_STRING + ".png")
                                                : Qt.resolvedUrl("../images/like.png")
                        enabled: (video.hasWriteScope) && (video.userId != YouTube.userId)
                        onClicked: if ((!video.liked) && (!video.disliked)) video.like();
                    }

                    Label {
                        height: likeButton.height
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: platformStyle.fontSizeSmall
                        text: video.likeCount
                    }

                    MyButton {
                        id: dislikeButton

                        width: height
                        iconSource: video.disliked ? Qt.resolvedUrl("../images/dislike-" + ACTIVE_COLOR_STRING + ".png")
                                                   : Qt.resolvedUrl("../images/dislike.png")
                        enabled: (video.hasWriteScope) && (video.userId != YouTube.userId)
                        onClicked: if ((!video.liked) && (!video.disliked)) video.dislike();
                    }

                    Label {
                        height: dislikeButton.height
                        verticalAlignment: Text.AlignVCenter
                        font.pixelSize: platformStyle.fontSizeSmall
                        text: video.dislikeCount
                    }

                    Label {
                        width: parent.width
                        wrapMode: Text.Wrap
                        text: video.description ? Utils.toRichText(video.description) : qsTr("No description")
                        onLinkActivated: {
                            var resource = Resources.getResourceFromUrl(link);

                            if (resource.service != Resources.YOUTUBE) {
                                Qt.openUrlExternally(link);
                                return;
                            }

                            if (resource.type == Resources.USER) {
                                appWindow.pageStack.push(Qt.resolvedUrl("YouTubeUserPage.qml")).load(resource.id);
                            }
                            else if (resource.type == Resources.PLAYLIST) {
                                appWindow.pageStack.push(Qt.resolvedUrl("YouTubePlaylistPage.qml")).load(resource.id);
                            }
                            else {
                                appWindow.pageStack.push(Qt.resolvedUrl("YouTubeVideoPage.qml")).load(resource.id);
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

                YouTubePlaybackDialog {
                    focusItem: flicker
                    onAccepted: VideoLauncher.playVideo(value.url)
                }
            }

            Component {
                id: downloadDialog

                YouTubeDownloadDialog {
                    focusItem: flicker
                }
            }

            Component {
                id: playlistDialog

                YouTubePlaylistDialog {
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
                YouTubeVideosTab {
                    Component.onCompleted: model.list("/search", ["snippet"], {relatedToVideoId: video.id},
                                                      {type: "video", maxResults: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: commentsTab

            width: view.width
            height: view.height
            title: qsTr("Comments")
            tab: Component {
                YouTubeCommentsTab {
                    Component.onCompleted: model.list("/commentThreads", ["snippet", "replies"], {videoId: video.id},
                                                      {textFormat: "plainText", maxResults: MAX_RESULTS})
                }
            }
        }
    }
}
