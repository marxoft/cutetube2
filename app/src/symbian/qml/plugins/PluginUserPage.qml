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
import ".."

MyPage {
    id: root

    function load(userOrId) {
        if (userOrId.hasOwnProperty("id")) {
            user.loadUser(userOrId);
        }
        else {
            user.loadUser(Settings.currentService, userOrId);
        }
    }

    title: view.currentTab.title
    showProgressIndicator: (user.status == ResourcesRequest.Loading)
                           || (view.currentTab.showProgressIndicator)
    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolButton {}
    }

    PluginUser {
        id: user

        onStatusChanged: if (status == ResourcesRequest.Failed) infoavatar.showMessage(errorString);
    }

    TabView {
        id: view

        anchors.fill: parent
        visible: user.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            title: user.username ? user.username + "'s " + qsTr("channel") : qsTr("Channel")

            Column {
                id: column

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: platformStyle.paddingLarge
                }

                spacing: platformStyle.paddingLarge

                Avatar {
                    id: avatar

                    width: height
                    height: Math.floor(parent.width / 4)
                    source: user.thumbnailUrl
                    enabled: false
                }

                Label {
                    id: nameLabel

                    width: parent.width
                    font.bold: true
                    text: user.username
                }
            }

            MyFlickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: column.bottom
                    topMargin: platformStyle.paddingLarge
                    bottom: parent.bottom
                }
                clip: true
                contentHeight: descriptionLabel.height + platformStyle.paddingLarge * 2

                Label {
                    id: descriptionLabel

                    anchors {
                        left: parent.left
                        leftMargin: platformStyle.paddingLarge
                        right: parent.right
                        rightMargin: platformStyle.paddingLarge
                        top: parent.top
                    }
                    wrapMode: Text.Wrap
                    text: user.description ? Utils.toRichText(user.description) : qsTr("No description")
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

            MyScrollBar {
                flickableItem: flicker
            }

            states: State {
                name: "landscape"
                when: !appWindow.inPortrait

                AnchorChanges {
                    target: column
                    anchors.right: parent.horizontalCenter
                }

                AnchorChanges {
                    target: flicker
                    anchors {
                        left: column.right
                        top: parent.top
                    }
                }
            }
        }

        TabLoader {
            id: uploadsTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("videos")
            tab: Component {
                PluginVideosTab {
                    Component.onCompleted: model.list(user.id)
                }
            }
        }

        TabLoader {
            id: playlistsTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("playlists")
            tab: Component {
                PluginPlaylistsTab {
                    Component.onCompleted: {
                        if (Plugins.resourceTypeIsSupported(Settings.currentService, Resources.PLAYLIST)) {
                            model.list(user.id);
                        }
                        else {
                            infoavatar.showMessage(qsTr("This channel does not have any playlists"));
                        }
                    }
                }
            }
        }
    }
}
