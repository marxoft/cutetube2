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
import QDailymotion 1.0 as QDailymotion
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    title: qsTr("Accounts")
    showProgressIndicator: (authRequest.status == QDailymotion.AuthenticationRequest.Loading)
                            || (userRequest.status == QDailymotion.ResourcesRequest.Loading)
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
        model: DailymotionAccountModel {
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

    QDailymotion.AuthenticationRequest {
        id: authRequest
        
        property bool revoke: false

        clientId: Dailymotion.clientId
        clientSecret: Dailymotion.clientSecret
        redirectUri: Dailymotion.redirectUri
        scopes: Dailymotion.scopes
        onFinished: {
            if (status == QDailymotion.AuthenticationRequest.Ready) {
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
                    userRequest.get("/me", {}, ["id", "screenname"]);
                    return;
                }
            }
            
            if (revoke) {
                loader.sourceComponent = errorDialog;
                loader.item.open();
            }
            else {
                infoBanner.showMessage(Dailymotion.getErrorString(result));
            }
        }
    }

    QDailymotion.ResourcesRequest {
        id: userRequest

        clientId: Dailymotion.clientId
        clientSecret: Dailymotion.clientSecret
        onFinished: {
            if (status == QDailymotion.ResourcesRequest.Ready) {
                if (result) {
                    if (accountModel.addAccount(result.id, result.screenname, accessToken, refreshToken,
                                                Dailymotion.scopes.join(" "))) {
                        infoBanner.showMessage(qsTr("You are signed in to account") + " '" + result.screenname + "'");
                    }
                    else {
                        infoBanner.showMessage(accountModel.errorString);
                    }

                    return;
                }
            }

            infoBanner.showMessage(Dailymotion.getErrorString(result));
        }
    }

    Component {
        id: authDialog

        DailymotionAuthDialog {
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
