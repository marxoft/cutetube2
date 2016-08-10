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
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

TextField {
    id: root

    property bool clearButtonEnabled: false
    property alias actionLabel: sipatts.actionKeyLabel
    property alias actionIcon: sipatts.actionKeyIcon
    property alias actionEnabled: sipatts.actionKeyEnabled
    property alias actionHighlighted: sipatts.actionKeyHighlighted
    property alias rightMargin: style.paddingRight

    signal accepted

    Loader {
        id: loader

        z: 1000
        anchors {
            right: parent.right
            rightMargin: 5
            verticalCenter: parent.verticalCenter
        }
        sourceComponent: root.clearButtonEnabled ? clearButton : undefined
    }

    Component {
        id: clearButton

        Image {
            id: icon

            source: "image://theme/icon-m-input-clear"
            opacity: mouseArea.pressed ? UI.OPACITY_DISABLED : UI.OPACITY_ENABLED
            visible: root.text != ""

            MouseArea {
                id: mouseArea

                width: 60
                height: 60
                anchors.centerIn: parent
                onClicked: {
                    root.text = "";
                    root.platformCloseSoftwareInputPanel();
                }
            }
        }
    }

    platformStyle: TextFieldStyle {
        id: style

        backgroundSelected: "image://theme/" + Settings.activeColorString + "-meegotouch-textedit-background-selected"
        backgroundDisabled: "image://theme/" + Settings.activeColorString + "-meegotouch-textedit-background-disabled"
        paddingRight: loader.item ? loader.item.width : undefined
    }

    platformSipAttributes: SipAttributes {
        id: sipatts

        actionKeyEnabled: acceptableInput
        actionKeyHighlighted: true
        actionKeyLabel: qsTr("Done")
        actionKeyIcon: ""
    }

    Keys.onEnterPressed: {
        if (acceptableInput) {
            accepted();
        }

        event.accepted = true;
    }
    Keys.onReturnPressed: {
        if (acceptableInput) {
            accepted();
        }

        event.accepted = true;
    }
}
