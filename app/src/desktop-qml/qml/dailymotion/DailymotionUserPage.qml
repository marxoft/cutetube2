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

import QtQuick 2.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import cuteTube 2.0
import QDailymotion 1.0 as QDailymotion
import ".."

Page {
    id: root

    function load(userOrId) {
        user.loadUser(userOrId);

        if ((user.id) && (!user.subscribed) && (Dailymotion.userId)) {
            user.checkIfSubscribed();
        }
    }

    title: user.username ? user.username + "'s " + qsTr("channel") : qsTr("Channel")
    tools: ToolBarLayout {
        
        ToolButton {
            id: backButton
            
            text: qsTr("Back")
            tooltip: qsTr("Go back")
            iconName: "go-previous"
            visible: root.Stack.index > 0
            onClicked: pageStack.pop({immediate: true})
        }
        
        Label {
            Layout.fillWidth: true
            text: root.title
        }
        
        TabButton {
            id: profileButton
        
            text: qsTr("Profile")
            checkable: true
            checked: true
            exclusiveGroup: tabGroup
            onClicked: tabView.currentIndex = 0
        }
        
        TabButton {
            id: uploadsButton
        
            text: qsTr("Videos")
            checkable: true
            exclusiveGroup: tabGroup
            onClicked: tabView.currentIndex = 1
        }
        
        TabButton {
            id: favouritesButton
        
            text: qsTr("Favourites")
            checkable: true
            exclusiveGroup: tabGroup
            onClicked: tabView.currentIndex = 2
        }
        
        TabButton {
            id: playlistsButton
        
            text: qsTr("Playlists")
            checkable: true
            exclusiveGroup: tabGroup
            onClicked: tabView.currentIndex = 3
        }
        
        TabButton {
            id: subscriptionsButton
        
            text: qsTr("Subscriptions")
            checkable: true
            exclusiveGroup: tabGroup
            onClicked: tabView.currentIndex = 4
        }
        
        ToolButton {
            id: reloadButton
    
            text: qsTr("Reload")
            tooltip: qsTr("Reload")
            iconName: "view-refresh"
            enabled: tabView.currentIndex > 0
            onClicked: tabView.getTab(tabView.currentIndex).model.reload()
        }
    
        ViewModeButton {
            id: viewButton
            
            enabled: tabView.currentIndex > 0
        }
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
                messageBox.showError(errorString);
                break;
            default:
                break;
            }
        }
    }
    
    ExclusiveGroup {
        id: tabGroup
    }

    TabView {
        id: tabView

        anchors.fill: parent
        frameVisible: false
        tabsVisible: false
        visible: user.id != ""

        Tab {
            id: infoTab

            width: parent.width
            height: parent.height
            
            Item {
                anchors {
                    fill: parent
                    leftMargin: 10
                    rightMargin: 10
                    topMargin: 10
                }
                
                Banner {
                    id: banner

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: parent.top
                    }
                    height: Math.floor(width / 4)
                    avatarSource: user.thumbnailUrl
                    bannerSource: user.bannerUrl
                    enabled: false
                }

                GridLayout {
                    id: grid

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: banner.bottom
                        topMargin: 10
                    }
                    columns: 2
                    columnSpacing: 10
                    rowSpacing: 10

                    Label {
                        id: nameLabel

                        Layout.fillWidth: true
                        font.bold: true
                        text: user.username
                    }
                    
                    Button {
                        id: subscribeButton
                        
                        text: user.subscribed ? qsTr("Unsubscribe") : qsTr("Subscribe")
                        iconName: user.subscribed ? "dialog-no" : "dialog-yes"
                        enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_SUBSCRIPTIONS_SCOPE))
                                 && (user.id) && (user.id != Dailymotion.userId)
                        onClicked: user.subscribed ? user.unsubscribe() : user.subscribe()
                    }

                    Label {
                        id: statsLabel

                        Layout.columnSpan: 2
                        Layout.alignment: Qt.AlignRight
                        horizontalAlignment: Text.AlignRight
                        color: palette.mid
                        text: Utils.formatLargeNumber(user.subscriberCount) + " " + qsTr("subscribers")
                              + "\n" + Utils.formatLargeNumber(user.viewCount) + " " + qsTr("views")
                    }                    
                }
            
                ScrollView {
                    id: scrollView
                
                    anchors {
                        left: parent.left
                        right: parent.right
                        top: grid.bottom
                        topMargin: 10
                        bottom: parent.bottom
                    }
                    focus: true

                    Flickable {
                        id: flicker
                    
                        anchors.fill: parent
                        contentHeight: descriptionLabel.height + 10 * 2

                        Label {
                            id: descriptionLabel

                            anchors {
                                left: parent.left
                                right: parent.right
                                top: parent.top
                            }
                            text: user.description ? Utils.toRichText(user.description) : qsTr("No description")
                            wrapMode: Text.Wrap
                            onLinkActivated: {
                                var resource = Resources.getResourceFromUrl(link);

                                if (resource.service != Resources.DAILYMOTION) {
                                    Qt.openUrlExternally(link);
                                    return;
                                }

                                if (resource.type == Resources.USER) {
                                    pageStack.push(Qt.resolvedUrl("DailymotionUserPage.qml")).load(resource.id);
                                }
                                else if (resource.type == Resources.PLAYLIST) {
                                    pageStack.push(Qt.resolvedUrl("DailymotionPlaylistPage.qml")).load(resource.id);
                                }
                                else {
                                    pageStack.push(Qt.resolvedUrl("DailymotionVideoPage.qml")).load(resource.id);
                                }
                            }
                        }
                    }
                }
            }
        }

        DailymotionVideosTab {
            id: uploadsTab
            
            onLoaded: model.list("/user/" + user.id + "/videos",
                                 {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
        }

        DailymotionVideosTab {
            id: favouritesTab
            
            onLoaded: model.list("/user/" + user.id + "/favorites",
                                 {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
        }

        DailymotionPlaylistsTab {
            id: playlistsTab
            
            onLoaded: model.list("/user/" + user.id + "/playlists",
                                 {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
        }

        DailymotionUsersTab {
            id: subscriptionsTab
            
            onLoaded: model.list("/user/" + user.id + "/following",
                                 {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
        }
    }
}
