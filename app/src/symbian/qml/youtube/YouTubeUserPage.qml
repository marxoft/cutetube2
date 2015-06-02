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

    function load(userOrId) {
        user.loadUser(userOrId);

        if ((user.id) && (!user.subscribed) && (YouTube.userId)) {
            user.checkIfSubscribed();
        }
    }

    title: view.currentTab.title
    showProgressIndicator: (user.status == QYouTube.ResourcesRequest.Loading)
                           || (view.currentTab.showProgressIndicator)
    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolButton {}
    }

    YouTubeUser {
        id: user

        property bool hasWriteScope: (YouTube.userId) && ((YouTube.hasScope(YouTube.READ_WRITE_SCOPE))
                                                          || (YouTube.hasScope(YouTube.FORCE_SSL_SCOPE)))

        onStatusChanged: {
            switch (status) {
            case QYouTube.ResourcesRequest.Ready:
                if ((!subscribed) && (YouTube.userId)) {
                    checkIfSubscribed();
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

    TabView {
        id: view

        anchors.fill: parent
        visible: user.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            title: user.username ? user.username + "'s " + qsTr("channel") : qsTr("Channel")
            tools: ToolBarLayout {

                BackToolButton {}

                MyToolButton {
                    iconSource: user.subscribed ? "../images/close.png" : "toolbar-add"
                    toolTipText: user.subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                    enabled: (user.hasWriteScope) && (user.id) && (user.id != YouTube.userId)
                    onClicked: user.subscribed ? user.unsubscribe() : user.subscribe()
                }
            }

            Flow {
                id: flow

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: platformStyle.paddingLarge
                }

                spacing: platformStyle.paddingLarge

                Banner {
                    id: banner

                    width: parent.width
                    height: Math.floor(width / 4)
                    avatarSource: user.thumbnailUrl
                    bannerSource: user.bannerUrl
                    enabled: false
                }

                Label {
                    id: nameLabel

                    font.bold: true
                    text: user.username
                }

                Label {
                    id: statsLabel

                    width: parent.width - nameLabel.width - platformStyle.paddingLarge
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: platformStyle.fontSizeSmall
                    font.weight: Font.Light
                    color: platformStyle.colorNormalMid
                    text: Utils.formatLargeNumber(user.subscriberCount) + " " + qsTr("subscribers")
                          + "\n" + Utils.formatLargeNumber(user.viewCount) + " " + qsTr("views")
                }
            }

            MyFlickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: flow.bottom
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

            MyScrollBar {
                flickableItem: flicker
            }

            states: State {
                name: "landscape"
                when: !appWindow.inPortrait

                AnchorChanges {
                    target: flow
                    anchors.right: parent.horizontalCenter
                }

                AnchorChanges {
                    target: flicker
                    anchors {
                        left: flow.right
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
                YouTubeVideosTab {
                    Component.onCompleted: {
                        if (user.relatedPlaylists.uploads) {
                            model.list("/playlistItems", ["snippet"], {playlistId: user.relatedPlaylists.uploads},
                                       {maxResults: MAX_RESULTS});
                        }
                        else {
                            infoBanner.showMessage(qsTr("This channel does not have any videos"));
                        }
                    }
                }
            }
        }

        TabLoader {
            id: favouritesTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("favourites")
            tab: Component {
                YouTubeVideosTab {
                    Component.onCompleted: {
                        if (user.relatedPlaylists.favorites) {
                            model.list("/playlistItems", ["snippet"], {playlistId: user.relatedPlaylists.favorites},
                                       {maxResults: MAX_RESULTS});
                        }
                        else {
                            infoBanner.showMessage(qsTr("This channel does not have any favourites"));
                        }
                    }
                }
            }
        }

        TabLoader {
            id: playlistsTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("playlists")
            tab: Component {
                YouTubePlaylistsTab {
                    Component.onCompleted: model.list("/playlists", ["snippet", "contentDetails"], {channelId: user.id},
                                                      {maxResults: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: subscriptionsTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("subscriptions")
            tab: Component {
                YouTubeUsersTab {
                    Component.onCompleted: model.list("/subscriptions", ["snippet"], {channelId: user.id},
                                                      {sort: "unread", maxResults: MAX_RESULTS})
                }
            }
        }
    }
}
