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

Rectangle {
    id: root

    property bool acceptWhenDone: true
    property bool hideHeaderWhenInputContextIsVisible: true
    property bool showProgressIndicator: false
    property string acceptButtonText
    property string rejectButtonText
    property alias content: contentField.children
    property int status: DialogStatus.Closed
    property Item focusItem: null

    signal opened
    signal closed
    signal accepted
    signal rejected
    signal done

    function open() {
        root.status = DialogStatus.Open
        root.opened();
    }

    function close() {
        root.status = DialogStatus.Closed;
        root.closed();
    }

    function accept() {
        root.accepted();
        root.close();
    }

    function reject() {
        root.rejected();
        root.close();
    }

    z: Number.MAX_VALUE
    y: root.status === DialogStatus.Closed ? 640 : 0
    parent: appWindow.pageStack.parent
    width: !parent ? implicitWidth : parent.width
    height: !parent ? implicitHeight : parent.height
    color: false ? platformStyle.colorBackgroundInverted : platformStyle.colorBackground
    visible: y < 640

    Behavior on y { NumberAnimation { easing.type: Easing.OutQuint; duration: 300 } }

    ListHeading {
        id: header

        z: 999
        anchors {
            left: parent.left
            right: parent.right
            top: parent.top
        }
        height: privateStyle.tabBarHeightPortrait
        platformInverted: false

        MyButton {
            id: rejectButton

            width: appWindow.inPortrait ? 150 : 180
            anchors {
                left: parent.left
                leftMargin: platformStyle.paddingMedium
                verticalCenter: parent.verticalCenter
            }
            text: rejectButtonText
            visible: (header.height === privateStyle.tabBarHeightPortrait) && (rejectButtonText != "")
            onClicked: root.reject()
        }

        BusyIndicator {
            id: progressIndicator

            width: 28
            height: 28
            anchors.centerIn: parent
            running: visible
            visible: root.showProgressIndicator
        }

        MyButton {
            id: acceptButton

            width: appWindow.inPortrait ? 150 : 180
            anchors {
                right: parent.right
                rightMargin: platformStyle.paddingMedium
                verticalCenter: parent.verticalCenter
            }
            text: acceptButtonText
            visible: (header.height === privateStyle.tabBarHeightPortrait) && (acceptButtonText != "")
            onClicked: root.acceptWhenDone ? root.accept() : root.done()
        }

        Rectangle {
            height: 1
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            color: Settings.activeColor
            opacity: 0.5
        }
    }

    Item {
        id: contentField

        anchors {
            fill: parent
            topMargin: header.height
        }
        clip: true
    }

    Image {
        z: 1001
        visible: appWindow.cornersVisible
        anchors.top : parent.top
        anchors.left: parent.left
        source: "images/corner-left.png"
    }

    Image {
        z: 1001
        visible: appWindow.cornersVisible
        anchors.top: parent.top
        anchors.right: parent.right
        source: "images/corner-right.png"
    }

    MouseArea {
        z: -1
        anchors.fill: parent
    }

    StateGroup {
        states: State {
            name: "textInputMode"
            when: (inputContext.visible) && (hideHeaderWhenInputContextIsVisible)
            PropertyChanges { target: header; height: 0 }
        }

        transitions: Transition {
            NumberAnimation { target: header; properties: "height"; duration: 200 }
        }
    }

    onStatusChanged: if ((status === DialogStatus.Closed) && (focusItem)) focusItem.forceActiveFocus();
}
