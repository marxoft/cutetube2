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
import QVimeo 1.0 as QVimeo
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
            model: VimeoAccountModel {
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
                    if (accountModel.removeAccount(view.currentIndex) {
                        messageBox.showInformation(qsTr("Account removed. Please visit the Vimeo website to revoke the \
                        access token"));
                    }
                    else {
                        messageBox.showError(accountModel.errorString);
                    }
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
                onUrlChanged: {
                    var s = url.toString();

                    if (s == "https://vimeo.com/") {
                        // Work-around as for some reason Vimeo sends you to the homepage on the first attempt.
                        url = Vimeo.authUrl();
                    }
                    else if (/code=/i.test(s)) {
                        authRequest.exchangeCodeForAccessToken(s.split("code=")[1].split("&")[0]);
                        loader.sourceComponent = undefined;
                    }
                }
            
                Component.onCompleted: {
                    CookieJar.setAllCookies([]);
                    url = Vimeo.authUrl();
                }
            }
        }
    }

    QVimeo.AuthenticationRequest {
        id: authRequest

        clientId: Vimeo.clientId
        clientSecret: Vimeo.clientSecret
        redirect: Vimeo.redirectUri
        scopes: Vimeo.scopes
        onFinished: {
            if (status == QVimeo.AuthenticationRequest.Ready) {
                if ((result.user) && (result.access_token)) {
                    var user = result.user;
                    
                    if (accountModel.addAccount(user.uri.substring(user.uri.lastIndexOf("/")), user.name,
                                                result.accessToken, result.scope)) {
                        statusBar.showMessage(qsTr("You are signed in to account") + " '" + user.name + "'");
                    }
                    else {
                        messageBox.showError(accountModel.errorString);
                    }
                    
                    return;
                }
            }

            messageBox.showError(Vimeo.getErrorString(result));
        }
    }   
}
