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
import QDailymotion 1.0 as QDailymotion
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    property alias model: videoModel

    title: qsTr("Videos")
    showProgressIndicator: (video.status == QDailymotion.ResourcesRequest.Loading)
                           || (videoModel.status == QDailymotion.ResourcesRequest.Loading)
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-refresh"
            enabled: videoModel.status != QDailymotion.ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
    }

    DailymotionVideo {
        id: video

        onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
    }

    ListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        highlightFollowsCurrentItem: false
        model: DailymotionVideoModel {
            id: videoModel

            onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: VideoDelegate {
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("DailymotionVideoPage.qml")).load(videoModel.get(index))
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

    ScrollDecorator {
        flickableItem: view
    }

    Label {
        anchors {
            fill: parent
            margins: UI.PADDING_DOUBLE
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 60
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
        text: qsTr("No videos found")
        visible: (videoModel.status >= QDailymotion.ResourcesRequest.Ready) && (videoModel.count == 0)
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {

            MenuItem {
                text: qsTr("Download")
                onClicked: {
                    dialogLoader.sourceComponent = downloadDialog;
                    dialogLoader.item.list(videoModel.data(view.currentIndex, "id"),
                                           videoModel.data(view.currentIndex, "title"));
                    dialogLoader.item.open();
                }
            }

            MenuItem {
                text: qsTr("Share")
                onClicked: if (!ShareUi.shareVideo(videoModel.get(view.currentIndex)))
                               infoBanner.showMessage(qsTr("Unable to share video"));
            }

            MenuItem {
                text: videoModel.data(view.currentIndex, "favourited") ? qsTr("Unfavourite") : qsTr("Favourite")
                enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_FAVOURITES_SCOPE))
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
                text: qsTr("Add to playlist")
                enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_PLAYLISTS_SCOPE))
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

        DailymotionPlaybackDialog {
            onAccepted: VideoPlayer.playVideo(value.url)
        }
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
