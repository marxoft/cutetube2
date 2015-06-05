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
import ".."

Page {
    id: root

    function load(userOrId) {
        if (userOrId.hasOwnProperty("id")) {
            user.loadUser(userOrId);
        }
        else {
            user.loadUser(Settings.currentService, userOrId);
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
            id: playlistsButton
        
            text: qsTr("Playlists")
            checkable: true
            exclusiveGroup: tabGroup
            onClicked: tabView.currentIndex = 3
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

    PluginUser {
        id: user

        onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
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
                
                Avatar {
                    id: avatar

                    anchors {
                        left: parent.left
                        top: parent.top
                    }
                    width: Math.floor(parent.width / 4)
                    height: width
                    source: user.thumbnailUrl
                    enabled: false
                }

                GridLayout {
                    id: grid

                    anchors {
                        left: parent.left
                        right: parent.right
                        top: avatar.bottom
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

                                if (resource.service != Settings.currentService) {
                                    Qt.openUrlExternally(link);
                                    return;
                                }

                                if (resource.type == Resources.USER) {
                                    pageStack.push(Qt.resolvedUrl("PluginUserPage.qml")).load(resource.id);
                                }
                                else if (resource.type == Resources.PLAYLIST) {
                                    pageStack.push(Qt.resolvedUrl("PluginPlaylistPage.qml")).load(resource.id);
                                }
                                else {
                                    pageStack.push(Qt.resolvedUrl("PluginVideoPage.qml")).load(resource.id);
                                }
                            }
                        }
                    }
                }
            }
        }

        PluginVideosTab {
            id: uploadsTab
            
            onLoaded: model.list(user.id)
        }

        PluginPlaylistsTab {
            id: playlistsTab
            
            onLoaded: {
                if (Plugins.resourceTypeIsSupported(Settings.currentService, Resources.PLAYLIST)) {
                    model.list(user.id);
                }
                else {
                    infoBanner.showMessage(qsTr("This channel does not have any playlists"));
                }
            }
        }
    }
}
