/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.meego 1.0
import cuteTube 2.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    title: qsTr("Network")
    tools: ToolBarLayout {

        BackToolIcon {
            onClicked: Settings.setNetworkProxy()
        }
    }

    Flickable {
        id: flicker

        anchors.fill: parent
        contentHeight: column.height + UI.PADDING_DOUBLE * 2

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: UI.PADDING_DOUBLE
            }
            spacing: UI.PADDING_DOUBLE

            MySwitch {
                x: -UI.PADDING_DOUBLE
                width: parent.width + UI.PADDING_DOUBLE * 2
                text: qsTr("Enable network proxy")
                checked: Settings.networkProxyEnabled
                onCheckedChanged: Settings.networkProxyEnabled = checked
            }

            ValueSelector {
                id: typeSelector

                x: -UI.PADDING_DOUBLE
                width: parent.width + UI.PADDING_DOUBLE * 2
                title: qsTr("Proxy type")
                model: NetworkProxyTypeModel {}
                value: Settings.networkProxyType
            }

            Label {
                font.bold: true
                text: qsTr("Host")
            }

            MyTextField {
                id: hostField

                width: parent.width
                text: Settings.networkProxyHost
                onTextChanged: Settings.networkProxyHost = text
                onAccepted: platformCloseSoftwareInputPanel()
            }

            Label {
                font.bold: true
                text: qsTr("Port")
            }

            MyTextField {
                id: portField

                width: parent.width
                text: Settings.networkProxyPort
                inputMethodHints: Qt.ImhDigitsOnly
                onTextChanged: Settings.networkProxyPort = text
                onAccepted: platformCloseSoftwareInputPanel()
            }

            Label {
                font.bold: true
                text: qsTr("Username")
            }

            MyTextField {
                id: userField

                width: parent.width
                text: Settings.networkProxyUsername
                onTextChanged: Settings.networkProxyUsername = text
                onAccepted: platformCloseSoftwareInputPanel()
            }

            Label {
                font.bold: true
                text: qsTr("Password")
            }

            MyTextField {
                id: passwordField

                width: parent.width
                echoMode: TextInput.Password
                text: Settings.networkProxyPassword
                onTextChanged: Settings.networkProxyPassword = text
                onAccepted: platformCloseSoftwareInputPanel()
            }
        }
    }

    ScrollDecorator {
        flickableItem: flicker
    }
}
