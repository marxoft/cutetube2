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

MySheet {
    id: root

    function load(videoOrId) {
        video.loadVideo(videoOrId);
    }

    acceptWhenDone: false
    showProgressIndicator: (playlist.status == QDailymotion.ResourcesRequest.Loading)
                           || (playlistModel.status == QDailymotion.ResourcesRequest.Loading)
    acceptButtonText: (view.currentTab == newPlaylistTab) && (titleField.text) ? qsTr("Done") : ""
    rejectButtonText: qsTr("Cancel")
    content: Item {

        anchors.fill: parent

        SeparatorLabel {
            id: separator

            anchors {
                left: parent.left
                leftMargin: UI.PADDING_DOUBLE
                right: parent.right
                top: parent.top
                topMargin: UI.PADDING_DOUBLE
            }
            text: view.currentTab.title
        }

        TabView {
            id: view

            anchors {
                left: parent.left
                right: parent.right
                top: separator.bottom
                topMargin: UI.PADDING_DOUBLE
                bottom: parent.bottom
            }
            enabled: playlist.status != QDailymotion.ResourcesRequest.Loading

            Tab {
                id: playlistsTab

                width: view.width
                height: view.height
                title: qsTr("Playlists")

                ListView {
                    id: playlistView

                    width: parent.width
                    height: parent.height
                    highlightFollowsCurrentItem: false
                    model: DailymotionPlaylistModel {
                        id: playlistModel

                        onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
                    }
                    delegate: PlaylistDelegate {
                        onClicked: {
                            playlist.loadPlaylist(playlistModel.get(index));
                            playlist.addVideo(video);
                        }
                    }
                }

                ScrollDecorator {
                    flickableItem: playlistView
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
                    text: qsTr("No playlists found")
                    visible: (playlistModel.status >= QDailymotion.ResourcesRequest.Ready) && (playlistModel.count == 0)
                }
            }

            Tab {
                id: newPlaylistTab

                width: view.width
                height: view.height
                title: qsTr("New playlist")

                Flickable {
                    id: flicker

                    width: parent.width
                    height: parent.height
                    contentHeight: column.height + UI.PADDING_DOUBLE * 2

                    Column {
                        id: column

                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            margins: UI.PADDING_DOUBLE
                        }
                        spacing: UI.PADDING_DOUBLE

                        Label {
                            font.bold: true
                            text: qsTr("Title")
                        }

                        MyTextField {
                            id: titleField

                            width: parent.width
                            onAccepted: platformCloseSoftwareInputPanel()
                        }

                        Label {
                            font.bold: true
                            text: qsTr("Description")
                        }

                        MyTextArea {
                            id: descriptionField

                            width: parent.width
                        }
                    }
                }
            }
        }
    }

    DailymotionVideo {
        id: video
    }

    DailymotionPlaylist {
        id: playlist

        onStatusChanged: {
            switch (status) {
            case QDailymotion.ResourcesRequest.Ready:
                root.accept();
                break;
            case QDailymotion.ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    onDone: {
        var p = {};
        p["name"] = titleField.text;

        if (descriptionField.text) {
            p["description"] = descriptionField.text;
        }

        playlist.addVideo(p, video);
    }

    onStatusChanged: {
        if (status == DialogStatus.Opening) {
            playlistModel.list("/me/playlists", {limit: MAX_RESULTS});
        }
        else if (status == DialogStatus.Closed) {
            titleField.text = "";
            descriptionField.text = "";
            view.currentIndex = 0;
        }
    }
}
