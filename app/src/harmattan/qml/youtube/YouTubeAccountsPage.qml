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
import QYouTube 1.0 as QYouTube
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    title: qsTr("Accounts")
    showProgressIndicator: (authRequest.status == QYouTube.AuthenticationRequest.Loading)
                            || (userRequest.status == QYouTube.ResourcesRequest.Loading)
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-add"
            onClicked: {
                loader.sourceComponent = authDialog;
                loader.item.open();
            }
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        highlightFollowsCurrentItem: false
        enabled: !root.showProgressIndicator
        model: YouTubeAccountModel {
            id: accountModel
        }
        delegate: LabelDelegate {
            text: username
            isSelected: active
            onClicked: infoBanner.showMessage(accountModel.selectAccount(index)
                                              ? qsTr("You have selected account") + " '" + username + "'"
                                              : accountModel.errorString)
            onPressAndHold: {
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
        text: qsTr("No accounts")
        visible: accountModel.count == 0
    }

    ContextMenu {
        id: contextMenu

        MenuLayout {

            MenuItem {
                text: qsTr("Remove")
                onClicked: {
                    authRequest.accessToken = accountModel.data(view.currentIndex, "accessToken");
                    authRequest.refreshToken = accountModel.data(view.currentIndex, "refreshToken");
                    authRequest.revoke = true;
                    authRequest.revokeAccessToken();
                }
            }
        }
    }

    Loader {
        id: loader
    }

    QYouTube.AuthenticationRequest {
        id: authRequest
        
        property bool revoke: false

        apiKey: YouTube.apiKey
        clientId: YouTube.clientId
        clientSecret: YouTube.clientSecret
        scopes: YouTube.scopes
        onFinished: {
            if (status == QYouTube.AuthenticationRequest.Ready) {
                if (revoke) {
                    var username = accountModel.data(view.currentIndex, "username");
                    infoBanner.showMessage(accountModel.removeAccount(view.currentIndex)
                                           ? qsTr("Account") + " '" + username + "' " + qsTr("removed")
                                           : accountModel.errorString);
                    return;
                }

                if (result.access_token) {
                    userRequest.accessToken = result.access_token;
                    userRequest.refreshToken = result.refresh_token;
                    userRequest.list("/channels", ["id", "snippet", "contentDetails"], {mine: true});
                    return;
                }
            }

            if (revoke) {
                loader.sourceComponent = errorDialog;
                loader.item.open();
            }
            else {
                infoBanner.showMessage(YouTube.getErrorString(result));
            }
        }
    }

    QYouTube.ResourcesRequest {
        id: userRequest

        apiKey: YouTube.apiKey
        clientId: YouTube.clientId
        clientSecret: YouTube.clientSecret
        onFinished: {
            if (status == QYouTube.ResourcesRequest.Ready) {
                if (result.items.length > 0) {
                    var user = result.items[0];

                    var username = user.snippet.title;

                    if (accountModel.addAccount(user.id, username, user.contentDetails.relatedPlaylists,
                                                accessToken, refreshToken, YouTube.scopes.join(" "))) {
                        infoBanner.showMessage(qsTr("You are signed in to account") + " '" + username + "'");
                    }
                    else {
                        infoBanner.showMessage(accountModel.errorString);
                    }

                    return;
                }
            }

            infoBanner.showMessage(YouTube.getErrorString(result));
        }
    }

    Component {
        id: authDialog

        YouTubeAuthDialog {
            onCodeReady: {
                authRequest.revoke = false;
                authRequest.exchangeCodeForAccessToken(code);
            }
        }
    }
    
    Component {
        id: errorDialog
        
        QueryDialog {
            titleText: qsTr("Error")
            message: qsTr("Unable to revoke access token. Do you want to remove the account anyway?")
            acceptButtonText: qsTr("Yes")
            rejectButtonText: qsTr("No")
            onAccepted: {
                var username = accountModel.data(view.currentIndex, "username");
                infoBanner.showMessage(accountModel.removeAccount(view.currentIndex)
                                       ? qsTr("Account") + " '" + username + "' " + qsTr("removed")
                                       : accountModel.errorString);
            }
        }
    }
}
