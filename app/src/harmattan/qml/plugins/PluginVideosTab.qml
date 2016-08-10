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
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Tab {
    id: root

    property alias model: videoModel

    title: qsTr("Videos")
    showProgressIndicator: videoModel.status == ResourcesRequest.Loading
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-refresh"
            enabled: videoModel.status != ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        highlightFollowsCurrentItem: false
        model: PluginVideoModel {
            id: videoModel

            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: VideoDelegate {
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginVideoPage.qml")).load(videoModel.get(index))
            onThumbnailClicked: {
                if (Settings.videoPlayer == "cutetube") {
                    appWindow.pageStack.push(Qt.resolvedUrl("../VideoPlaybackPage.qml")).addVideos([videoModel.get(index)]);
                }
                else {
                    var streamUrl = videoModel.data(index, "streamUrl").toString();

                    if (streamUrl) {
                        VideoLauncher.playVideo(streamUrl);
                    }
                    else {
                        dialogLoader.sourceComponent = playbackDialog;
                        dialogLoader.item.model.list(id);
                        dialogLoader.item.open();
                    }
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
        visible: (videoModel.status >= ResourcesRequest.Ready) && (videoModel.count == 0)
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {

            MenuItem {
                text: qsTr("Download")
                enabled: (view.currentIndex >= 0) && (videoModel.data(view.currentIndex, "downloadable"))
                onClicked: {
                    dialogLoader.sourceComponent = downloadDialog;
                    dialogLoader.item.videoId = videoModel.data(view.currentIndex, "id");
                    dialogLoader.item.videoTitle = videoModel.data(view.currentIndex, "title");
                    dialogLoader.item.streamUrl = videoModel.data(view.currentIndex, "streamUrl");
                    dialogLoader.item.listSubtitles = videoModel.data(view.currentIndex, "subtitles");
                    dialogLoader.item.open();
                }
            }

            MenuItem {
                text: qsTr("Share")
                onClicked: if (!ShareUi.shareVideo(videoModel.get(view.currentIndex)))
                               infoBanner.showMessage(qsTr("Unable to share video"));
            }
        }
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: playbackDialog

        PluginPlaybackDialog {
            onAccepted: VideoLauncher.playVideo(value.url)
        }
    }

    Component {
        id: downloadDialog

        PluginDownloadDialog {}
    }
}
