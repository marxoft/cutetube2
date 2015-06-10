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
import QYouTube 1.0 as QYouTube
import ".."

MyPage {
    id: root

    property alias model: videoModel

    title: qsTr("Videos")
    showProgressIndicator: (video.status == QYouTube.ResourcesRequest.Loading)
                           || (videoModel.status == QYouTube.ResourcesRequest.Loading)
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-refresh"
            toolTipText: qsTr("Reload")
            enabled: videoModel.status != QYouTube.ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
    }

    YouTubeVideo {
        id: video

        property bool hasWriteScope: (YouTube.userId) && ((YouTube.hasScope(YouTube.READ_WRITE_SCOPE))
                                                          || (YouTube.hasScope(YouTube.FORCE_SSL_SCOPE)))

        onStatusChanged: if (status == QYouTube.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    MyListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        model: YouTubeVideoModel {
            id: videoModel

            onStatusChanged: if (status == QYouTube.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: VideoDelegate {
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("YouTubeVideoPage.qml")).load(videoModel.get(index))
            onThumbnailClicked: {
                if (Settings.videoPlayer == "cutetube") {
                    appWindow.pageStack.push(Qt.resolvedUrl("../VideoPlaybackPage.qml")).addVideos([videoModel.get(index)]);
                }
                else {
                    dialogLoader.sourceComponent = playbackDialog;
                    dialogLoader.item.model.list(id);
                    dialogLoader.item.open();
                }
            }

            onPressAndHold: {
                view.currentIndex = -1;
                view.currentIndex = index;
                contextMenu.open();
            }
        }
    }

    MyScrollBar {
        flickableItem: view
    }

    Label {
        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 40
        color: platformStyle.colorNormalMid
        text: qsTr("No videos found")
        visible: (videoModel.status >= QYouTube.ResourcesRequest.Ready) && (videoModel.count == 0)
    }

    MyContextMenu {
        id: contextMenu

        focusItem: view

        MenuLayout {

            MenuItem {
                text: qsTr("Download")
                onClicked: {
                    dialogLoader.sourceComponent = downloadDialog;
                    dialogLoader.item.resourceId = videoModel.data(view.currentIndex, "id");
                    dialogLoader.item.resourceTitle = videoModel.data(view.currentIndex, "title");
                    dialogLoader.item.open();
                }
            }

            MenuItem {
                text: qsTr("Copy URL")
                onClicked: {
                    Clipboard.text = videoModel.data(view.currentIndex, "url");
                    infoBanner.showMessage(qsTr("URL copied to clipboard"));
                }
            }

            MenuItem {
                text: videoModel.data(view.currentIndex, "favourited") ? qsTr("Unfavourite") : qsTr("Favourite")
                enabled: video.hasWriteScope
                onClicked: {
                    video.loadVideo(videoModel.get(view.currentIndex))

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
                enabled: video.hasWriteScope
                onClicked: {
                    video.loadVideo(videoModel.get(view.currentIndex))
                    video.watchLater();
                }
            }

            MenuItem {
                text: qsTr("Add to playlist")
                enabled: video.hasWriteScope
                onClicked: {
                    dialogLoader.sourceComponent = playlistDialog;
                    dialogLoader.item.load(videoModel.get(view.currentIndex));
                    dialogLoader.item.open();
                }
            }
        }
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: playbackDialog

        YouTubePlaybackDialog {
            focusItem: view
            onAccepted: VideoLauncher.playVideo(value.url)
        }
    }

    Component {
        id: downloadDialog

        YouTubeDownloadDialog {
            focusItem: view
        }
    }

    Component {
        id: playlistDialog

        YouTubePlaylistDialog {
            focusItem: view
        }
    }
}
