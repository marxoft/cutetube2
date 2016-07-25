/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1
import cuteTube 2.0

MyPage {
    id: root

    title: qsTr("Network")
    tools: ToolBarLayout {

        BackToolButton {
            onClicked: Settings.setNetworkProxy()
        }
    }

    KeyNavFlickable {
        id: flicker

        anchors.fill: parent
        contentHeight: column.height + platformStyle.paddingLarge * 2

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge

            MySwitch {
                x: -platformStyle.paddingLarge
                width: parent.width + platformStyle.paddingLarge * 2
                text: qsTr("Enable network proxy")
                visible: !inputContext.visible
                checked: Settings.networkProxyEnabled
                onCheckedChanged: Settings.networkProxyEnabled = checked
            }

            ValueSelector {
                id: typeSelector

                x: -platformStyle.paddingLarge
                width: parent.width + platformStyle.paddingLarge * 2
                title: qsTr("Proxy type")
                model: NetworkProxyTypeModel {}
                focusItem: flicker
                visible: !inputContext.visible
                value: Settings.networkProxyType
                onValueChanged: Settings.networkProxyType = value
            }

            Label {
                font.bold: true
                text: qsTr("Host")
                visible: hostField.visible
            }

            MyTextField {
                id: hostField

                width: parent.width
                text: Settings.networkProxyHost
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                visible: (!inputContext.visible) || (focus)
                onTextChanged: Settings.networkProxyHost = text
                onAccepted: closeSoftwareInputPanel()
            }

            Label {
                font.bold: true
                text: qsTr("Port")
                visible: portField.visible
            }

            MyTextField {
                id: portField

                width: parent.width
                text: Settings.networkProxyPort
                inputMethodHints: Qt.ImhDigitsOnly
                visible: (!inputContext.visible) || (focus)
                onTextChanged: Settings.networkProxyPort = text
                onAccepted: closeSoftwareInputPanel()
            }

            Label {
                font.bold: true
                text: qsTr("Username")
                visible: userField.visible
            }

            MyTextField {
                id: userField

                width: parent.width
                text: Settings.networkProxyUsername
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                visible: (!inputContext.visible) || (focus)
                onTextChanged: Settings.networkProxyUsername = text
                onAccepted: closeSoftwareInputPanel()
            }

            Label {
                font.bold: true
                text: qsTr("Password")
                visible: passwordField.visible
            }

            MyTextField {
                id: passwordField

                width: parent.width
                echoMode: TextInput.Password
                text: Settings.networkProxyPassword
                visible: (!inputContext.visible) || (focus)
                onTextChanged: Settings.networkProxyPassword = text
                onAccepted: closeSoftwareInputPanel()
            }
        }
    }

    MyScrollBar {
        flickableItem: flicker
    }
}
