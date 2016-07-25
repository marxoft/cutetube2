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
import com.nokia.symbian 1.1

TextField {
    id: root

    property bool clearButtonEnabled: false

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

            source: "images/input-clear.png"
            opacity: mouseArea.pressed ? 0.5 : 1
            visible: root.text != ""

            MouseArea {
                id: mouseArea

                width: height
                height: root.height
                anchors.centerIn: parent
                onClicked: {
                    root.text = "";
                    root.closeSoftwareInputPanel();
                }
            }
        }
    }

    platformRightMargin: loader.item ? loader.item.width : platformLeftMargin

    Keys.onEnterPressed: {
        if (acceptableInput) {
            accepted();
        }
    }
    Keys.onReturnPressed: {
        if (acceptableInput) {
            accepted();
        }
    }
}
