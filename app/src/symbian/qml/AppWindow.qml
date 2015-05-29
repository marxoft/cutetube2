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

import QtQuick 1.1
import com.nokia.symbian 1.1

Window {
    id: window

    property bool showStatusBar: true
    property bool showToolBar: true
    property variant initialPage
    property alias pageStack: stack
    property bool cornersVisible: true

    objectName: "pageStackWindow"

    StatusBar {
        id: statusBar

        anchors {
            top: parent.top
            topMargin: window.showStatusBar ? 0 : -height
        }
        width: parent.width
    }

    Item {
        id: contentItem

        objectName: "appWindowContent"
        width: parent.width
        anchors.top: window.showStatusBar ? statusBar.bottom : parent.top
        anchors.bottom: parent.bottom

        ToolBar {
            id: toolBar

            anchors.bottom: parent.bottom
            states: State {
                name: "hide"
                when: (!window.showToolBar) || (inputContext.softwareInputPanelVisible) || (inputContext.customSoftwareInputPanelVisible)
                PropertyChanges { target: toolBar; height: 0; opacity: 0.0 }
            }
        }

        PageStack {
            id: stack

            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: toolBar.top
            }
            clip: true
            toolBar: toolBar
        }

        Image {
            z: 1001
            visible: window.cornersVisible
            anchors.top : parent.top
            anchors.left: parent.left
            source: "images/corner-left.png"
        }

        Image {
            z: 1001
            visible: window.cornersVisible
            anchors.top: parent.top
            anchors.right: parent.right
            source: "images/corner-right.png"
        }
    }

    // event preventer when page transition is active
    MouseArea {
        anchors.fill: parent
        enabled: pageStack.busy
    }

    Component.onCompleted: if (initialPage) pageStack.push(initialPage);
}
