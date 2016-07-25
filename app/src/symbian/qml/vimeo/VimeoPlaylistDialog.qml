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
import QVimeo 1.0 as QVimeo
import ".."

MySheet {
    id: root

    function load(videoOrId) {
        video.loadVideo(videoOrId);
    }

    acceptWhenDone: false
    showProgressIndicator: (playlist.status == QVimeo.ResourcesRequest.Loading)
                           || (playlistModel.status == QVimeo.ResourcesRequest.Loading)
    acceptButtonText: (view.currentTab == newPlaylistTab) && (titleField.text) ? qsTr("Done") : ""
    rejectButtonText: qsTr("Cancel")
    content: Item {

        anchors.fill: parent

        SeparatorLabel {
            id: separator

            anchors {
                left: parent.left
                leftMargin: platformStyle.paddingLarge
                right: parent.right
                top: parent.top
                topMargin: platformStyle.paddingLarge
            }
            text: view.currentTab.title
        }

        TabView {
            id: view

            anchors {
                left: parent.left
                right: parent.right
                top: separator.bottom
                topMargin: platformStyle.paddingLarge
                bottom: parent.bottom
            }
            enabled: playlist.status != QVimeo.ResourcesRequest.Loading

            Tab {
                id: playlistsTab

                width: view.width
                height: view.height
                title: qsTr("Albums")

                MyListView {
                    id: playlistView

                    width: parent.width
                    height: parent.height
                    model: VimeoPlaylistModel {
                        id: playlistModel

                        onStatusChanged: if (status == QVimeo.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
                    }
                    delegate: PlaylistDelegate {
                        onClicked: {
                            playlist.loadPlaylist(playlistModel.get(index));
                            playlist.addVideo(video);
                        }
                    }
                }

                MyScrollBar {
                    flickableItem: playlistView
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
                    text: qsTr("No albums found")
                    visible: (playlistModel.status >= QVimeo.ResourcesRequest.Ready) && (playlistModel.count == 0)
                }
            }

            Tab {
                id: newPlaylistTab

                width: view.width
                height: view.height
                title: qsTr("New album")

                KeyNavFlickable {
                    id: flicker

                    width: parent.width
                    height: parent.height
                    contentHeight: column.height + platformStyle.paddingLarge * 2

                    Column {
                        id: column

                        anchors {
                            left: parent.left
                            right: parent.right
                            top: parent.top
                            margins: platformStyle.paddingLarge
                        }
                        spacing: platformStyle.paddingLarge

                        Label {
                            font.bold: true
                            text: qsTr("Title")
                            visible: titleField.visible
                        }

                        MyTextField {
                            id: titleField

                            width: parent.width
                            visible: (!inputContext.visible) || (focus)
                            onAccepted: closeSoftwareInputPanel()
                        }

                        Label {
                            font.bold: true
                            text: qsTr("Description")
                            visible: descriptionField.visible
                        }

                        TextArea {
                            id: descriptionField

                            width: parent.width
                            visible: (!inputContext.visible) || (focus)
                        }

                        Label {
                            font.bold: true
                            text: qsTr("Password (if private)")
                            visible: passwordField.visible
                        }

                        MyTextField {
                            id: passwordField

                            width: parent.width
                            echoMode: TextInput.Password
                            visible: (!inputContext.visible) || (focus)
                            onAccepted: closeSoftwareInputPanel()
                        }
                    }
                }

                MyScrollBar {
                    flickableItem: flicker
                }
            }
        }
    }

    VimeoVideo {
        id: video
    }

    VimeoPlaylist {
        id: playlist

        onStatusChanged: {
            switch (status) {
            case QVimeo.ResourcesRequest.Ready:
                root.accept();
                break;
            case QVimeo.ResourcesRequest.Failed:
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

        if (passwordField.text) {
            p["privacy"] = "password";
            p["password"] = passwordField.text;
        }

        playlist.addVideo(p, video);
    }

    onStatusChanged: {
        if (status == DialogStatus.Open) {
            playlistModel.list("/me/albums", {per_page: MAX_RESULTS});
        }
        else if (status == DialogStatus.Closed) {
            titleField.text = "";
            descriptionField.text = "";
            passwordField.text = "";
            view.currentIndex = 0;
        }
    }
}
