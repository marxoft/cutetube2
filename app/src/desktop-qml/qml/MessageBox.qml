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
    
    property alias text: label.text
    
    function showMessage(message) {
        title = qsTr("Information");
        text = message;
        open();
    }
    
    function showError(error) {
        title = qsTr("Error");
        text = error;
        open();
    }
    
    minimumWidth: 400
    minimumHeight: label.height + 60
    title: qsTr("Information")
    content: Label {
        id: label
        
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
    }
    buttons: Button {
        text: qsTr("&Ok")
        iconName: "dialog-ok"
        isDefault: true
        onClicked: root.accept()
    }
}
