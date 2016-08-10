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
import com.nokia.meego 1.0

Window {
    id: window

    property bool showStatusBar: true
    property bool showToolBar: true
    property variant initialPage
    property alias pageStack: stack
    property Style platformStyle: PageStackWindowStyle{}

    //Deprecated, TODO Remove this on w13
    property alias style: window.platformStyle

    objectName: "pageStackWindow"

    StatusBar {
        id: statusBar
        anchors {
            top: parent.top
            topMargin: window.showStatusBar ? 0 : -height
        }
        width: parent.width
    }

    onOrientationChangeStarted: statusBar.orientation = screen.currentOrientation

    Image {
        id: backgroundImage
        source: window.inPortrait ? platformStyle.portraiteBackground : platformStyle.landscapeBackground
        fillMode: platformStyle.backgroundFillMode
        width: window.inPortrait ? screen.displayHeight : screen.displayWidth
        height: window.inPortrait ? screen.displayWidth : screen.displayHeight
        anchors { top: statusBar.bottom; left: parent.left; }
    }

    Item {
        objectName: "appWindowContent"
        width: parent.width
        anchors.top: window.showStatusBar ? statusBar.bottom : parent.top
        anchors.bottom: parent.bottom

        // content area
        Item {
            id: contentArea
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: toolBar.top
            }
            PageStack {
                id: stack
                anchors.fill: parent
                toolBar: toolBar
            }
        }

        Image {
            z: 1001
            visible: platformStyle.cornersVisible
            anchors.top : parent.top
            anchors.left: parent.left
            source: "image://theme/meegotouch-applicationwindow-corner-top-left"
        }

        Image {
            z: 1001
            visible: platformStyle.cornersVisible
            anchors.top: parent.top
            anchors.right: parent.right
            source: "image://theme/meegotouch-applicationwindow-corner-top-right"
        }

        Image {
            z: 1001
            visible: (platformStyle.cornersVisible) || (window.inPortrait)
            anchors.bottom : parent.bottom
            anchors.left: parent.left
            source: "image://theme/meegotouch-applicationwindow-corner-bottom-left"
        }

        Image {
            z: 1001
            visible: (platformStyle.cornersVisible) || (window.inPortrait)
            anchors.bottom : parent.bottom
            anchors.right: parent.right
            source: "image://theme/meegotouch-applicationwindow-corner-bottom-right"
        }

        ToolBar {
            id: toolBar

            anchors.bottom: parent.bottom
            states: State {
                name: "hide"
                when: (!window.showToolBar) || (inputContext.softwareInputPanelVisible) || (inputContext.customSoftwareInputPanelVisible)
                PropertyChanges { target: toolBar; height: 0; opacity: 0.0 }
            }
        }
    }

    // event preventer when page transition is active
    MouseArea {
        anchors.fill: parent
        enabled: pageStack.busy
    }

    Component.onCompleted: {
        if (initialPage) pageStack.push(initialPage);
    }
}
