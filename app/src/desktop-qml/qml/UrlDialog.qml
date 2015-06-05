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

import QtQuick 2.0
import QtQuick.Controls 1.1

MyDialog {
    id: root
    
    property alias url: urlField.text
    
    minimumWidth: 400
    minimumHeight: urlField.height + 60
    title: qsTr("Open URL")
    content: TextField {
        id: urlField
        
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        placeholderText: qsTr("URL")
        validator: RegExpValidator {
            regExp: /^.+/
        }
        onAccepted: root.accept()
    }
    buttons: [
        Button {
            text: qsTr("&Cancel")
            iconName: "dialog-cancel"
            onClicked: root.reject()
        },
        
        Button {
            text: qsTr("&Ok")
            iconName: "dialog-ok"
            isDefault: true
            enabled: urlField.acceptableInput
            onClicked: root.accept()
        }
    ]
    
    onOpened: {
        urlField.text = "";
        urlField.forceActiveFocus();
    }
}
