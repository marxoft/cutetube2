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

    function load(userOrId) {
        user.loadUser(userOrId);

        if ((user.id) && (!user.subscribed) && (Dailymotion.userId)) {
            user.checkIfSubscribed();
        }
    }

    title: view.currentTab.title
    showProgressIndicator: (user.status == QDailymotion.ResourcesRequest.Loading)
                           || (view.currentTab.showProgressIndicator)
    tools: view.currentTab.tools ? view.currentTab.tools : ownTools
    onToolsChanged: if (status == PageStatus.Active) appWindow.pageStack.toolBar.setTools(tools, "set");

    ToolBarLayout {
        id: ownTools

        visible: false

        BackToolIcon {}
    }

    DailymotionUser {
        id: user

        onStatusChanged: {
            switch (status) {
            case QDailymotion.ResourcesRequest.Ready:
                if ((!subscribed) && (Dailymotion.userId)) {
                    checkIfSubscribed();
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

    TabView {
        id: view

        anchors.fill: parent
        visible: user.id != ""

        Tab {
            id: infoTab

            width: view.width
            height: view.height
            title: user.username ? user.username + "'s " + qsTr("channel") : qsTr("Channel")

            Flow {
                id: flow

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                    margins: UI.PADDING_DOUBLE
                }

                spacing: UI.PADDING_DOUBLE

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

                    width: parent.width - nameLabel.width - UI.PADDING_DOUBLE
                    horizontalAlignment: Text.AlignRight
                    font.pixelSize: UI.FONT_SMALL
                    font.family: UI.FONT_FAMILY_LIGHT
                    text: Utils.formatLargeNumber(user.subscriberCount) + " " + qsTr("subscribers")
                          + "\n" + Utils.formatLargeNumber(user.viewCount) + " " + qsTr("views")
                }

                MyButton {
                    id: subscribeButton

                    width: 250
                    text: user.subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                    enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_SUBSCRIPTIONS_SCOPE))
                             && (user.id) && (user.id != Dailymotion.userId)
                    onClicked: user.subscribed ? user.unsubscribe() : user.subscribe()
                }
            }

            Flickable {
                id: flicker

                anchors {
                    left: parent.left
                    right: parent.right
                    top: flow.bottom
                    topMargin: UI.PADDING_DOUBLE
                    bottom: parent.bottom
                }
                clip: true
                contentHeight: descriptionLabel.height + UI.PADDING_DOUBLE * 2

                Label {
                    id: descriptionLabel

                    anchors {
                        left: parent.left
                        leftMargin: UI.PADDING_DOUBLE
                        right: parent.right
                        rightMargin: UI.PADDING_DOUBLE
                        top: parent.top
                    }
                    text: user.description ? Utils.toRichText(user.description) : qsTr("No description")
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

            ScrollDecorator {
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
                DailymotionVideosTab {
                    Component.onCompleted: model.list("/user/" + user.id + "/videos",
                                                      {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: favouritesTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("favourites")
            tab: Component {
                DailymotionVideosTab {
                    Component.onCompleted: model.list("/user/" + user.id + "/favorites",
                                                      {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: playlistsTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("playlists")
            tab: Component {
                DailymotionPlaylistsTab {
                    Component.onCompleted: model.list("/user/" + user.id + "/playlists",
                                                      {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                }
            }
        }

        TabLoader {
            id: subscriptionsTab

            width: view.width
            height: view.height
            title: user.username + "'s " + qsTr("subscriptions")
            tab: Component {
                DailymotionUsersTab {
                    Component.onCompleted: model.list("/user/" + user.id + "/following",
                                                      {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                }
            }
        }
    }
}
