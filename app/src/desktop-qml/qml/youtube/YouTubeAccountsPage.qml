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
import QtWebKit.experimental 1.0
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import cuteTube 2.0
import QYouTube 1.0 as QYouTube
import ".."

Page {
    id: root

    title: qsTr("Accounts")
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
            id: label
            
            Layout.fillWidth: true
            text: root.title
        }
        
        ToolButton {
            id: reloadButton
        
            text: qsTr("Add account")
            tooltip: qsTr("Add account")
            iconName: "list-add"
            enabled: loader.sourceComponent == undefined
            onClicked: loader.sourceComponent = authView
        }
    }
    
    Item {
        anchors.fill: parent

        ItemView {
            id: view

            anchors.fill: parent
            model: YouTubeAccountModel {
                id: accountModel
            }
            delegate: LabelDelegate {
                text: username
                onClicked: {
                    if (accountModel.selectAccount(index)) {
                        statusBar.showMessage(qsTr("You have selected account") + " '" + username + "'");
                    }
                    else {
                        messageBox.showError(accountModel.errorString);
                    }
                }
                onRightClicked: contextMenu.popup()
            }
        }
        
        Label {
            anchors {
                fill: parent
                margins: 10
            }
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap
            font.pixelSize: 24
            color: palette.mid
            text: qsTr("No accounts")
            visible: accountModel.count == 0
        }
        
        Menu {
            id: contextMenu

            MenuItem {
                text: qsTr("Remove")
                onTriggered: {
                    authRequest.accessToken = accountModel.data(view.currentIndex, "accessToken");
                    authRequest.refreshToken = accountModel.data(view.currentIndex, "refreshToken");
                    authRequest.revokeAccessToken();
                }
            }
        }
    }
    
    Loader {
        id: loader
        
        anchors.fill: parent
    }
    
    Component {
        id: authView
        
        ScrollView {
            anchors.fill: parent
            
            WebView {
                id: webView

                anchors.fill: parent
                onTitleChanged: {
                    if (/code=/i.test(title)) {
                        authRequest.exchangeCodeForAccessToken(title.split("code=")[1].split("&")[0]);
                        loader.sourceComponent = undefined;
                    }
                }
            
                Component.onCompleted: {
                    CookieJar.setAllCookies([]);
                    url = YouTube.authUrl();
                }
            }
        }
    }

    QYouTube.AuthenticationRequest {
        id: authRequest

        apiKey: YouTube.apiKey
        clientId: YouTube.clientId
        clientSecret: YouTube.clientSecret
        scopes: YouTube.scopes
        onFinished: {
            if (status == QYouTube.AuthenticationRequest.Ready) {
                if (!result) {
                    var username = accountModel.data(view.currentIndex, "username");
                    
                    if (accountModel.removeAccount(view.currentIndex)) {
                        statusBar.showMessage(qsTr("Account") + " '" + username + "' " + qsTr("removed"));
                    }
                    else {
                        messageBox.showError(accountModel.errorString);
                    }
                    
                    return;
                }

                if (result.access_token) {
                    userRequest.accessToken = result.access_token;
                    userRequest.refreshToken = result.refresh_token;
                    userRequest.list("/channels", ["id", "snippet", "contentDetails"], {mine: true});
                    return;
                }
            }

            messageBox.showError(YouTube.getErrorString(result));
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
                        statusBar.showMessage(qsTr("You are signed in to account") + " '" + username + "'");
                    }
                    else {
                        messageBox.showError(accountModel.errorString);
                    }

                    return;
                }
            }

            messageBox.showError(YouTube.getErrorString(result));
        }
    }    
}
