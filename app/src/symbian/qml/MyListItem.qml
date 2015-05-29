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

Item {
    id: root
    property string mode: "normal" // Read-only
    property alias paddingItem: paddingItem // Read-only
    property alias state: internal.state

    property bool enabled: true
    property bool subItemIndicator: false
    property bool platformInverted: false
    property bool flickableMode: false

    signal clicked
    signal pressAndHold
    signal keyPressed(int key)

    implicitWidth: ListView.view ? ListView.view.width : screen.width
    implicitHeight: platformStyle.graphicSizeLarge

    Item {
        id: background
        anchors.fill: parent

        Rectangle {
            height: 1
            color: root.platformInverted ? platformStyle.colorDisabledLightInverted
                                         : platformStyle.colorDisabledMid
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }
        }
        Loader {
            id: faderLoader
            opacity: 0
            anchors.fill: background
            sourceComponent: root.mode != "normal" && root.mode != "pressed" ? fader : undefined
        }

        BorderImage {
            id: highlight
            border {
                left: platformStyle.borderSizeMedium
                top: platformStyle.borderSizeMedium
                right: platformStyle.borderSizeMedium
                bottom: platformStyle.borderSizeMedium
            }
            opacity: 0
            anchors.fill: background
        }
    }

    Component {
        id: fader

        BorderImage {
            source: privateStyle.imagePath("qtg_fr_list_" + mode, root.platformInverted)
            border {
                left: platformStyle.borderSizeMedium
                top: platformStyle.borderSizeMedium
                right: platformStyle.borderSizeMedium
                bottom: platformStyle.borderSizeMedium
            }
        }
    }

    MouseArea {
        id: mouseArea
        z: 1
        anchors.fill: parent
        enabled: root.enabled
        onPressed: {
            symbian.listInteractionMode = Symbian.TouchInteraction
            internal.state = "Pressed"
        }
        onClicked: {
            internal.state = ""
            root.clicked()
        }
        onCanceled: {
            internal.state = "Canceled"
        }
        onPressAndHold: {
            internal.state = "PressAndHold"
        }
        onReleased: {
            internal.state = ""
        }
        onExited: {
            internal.state = ""
        }
    }

    Loader {
        id: iconLoader
        sourceComponent: root.subItemIndicator ? subItemIcon : undefined
        anchors {
            right: parent.right
            rightMargin: privateStyle.scrollBarThickness
            verticalCenter: parent.verticalCenter
        }
    }

    Component {
        id: subItemIcon

        Image {
            source: privateStyle.imagePath(
                root.enabled ? "qtg_graf_drill_down_indicator"
                             : "qtg_graf_drill_down_indicator_disabled",
                root.platformInverted)
            mirror: LayoutMirroring.enabled
            sourceSize.width: platformStyle.graphicSizeSmall
            sourceSize.height: platformStyle.graphicSizeSmall
        }
    }

    Keys.onReleased: {
        if (!event.isAutoRepeat && root.enabled) {
            if (event.key == Qt.Key_Select || event.key == Qt.Key_Return || event.key == Qt.Key_Enter) {
                event.accepted = !flickableMode
                internal.state = "Focused"
            }
        }
    }

    Keys.onPressed: {
        if (!event.isAutoRepeat) {
            switch (event.key) {
                case Qt.Key_Select:
                case Qt.Key_Enter:
                case Qt.Key_Return: {
                    if (symbian.listInteractionMode != Symbian.KeyNavigation)
                        symbian.listInteractionMode = Symbian.KeyNavigation
                    else
                        if (root.enabled) {
                            highlight.source = privateStyle.imagePath("qtg_fr_list_pressed",
                                                                      root.platformInverted)
                            highlight.opacity = 1
                            releasedEffect.restart()
                            root.clicked()
                        }
                    event.accepted = true
                    break
                }

                case Qt.Key_Space: {
                    if (symbian.listInteractionMode != Symbian.KeyNavigation)
                        symbian.listInteractionMode = Symbian.KeyNavigation
                    event.accepted = true
                    break;
                }

                case Qt.Key_Up: {
                    if (symbian.listInteractionMode != Symbian.KeyNavigation) {
                        symbian.listInteractionMode = Symbian.KeyNavigation
                        internal.state = "Focused"
                        if (!flickableMode)
                            ListView.view.positionViewAtIndex(index, ListView.Beginning)
                    } else
                        if (!flickableMode)
                            ListView.view.decrementCurrentIndex()
                    event.accepted = !flickableMode
                    break
                }

                case Qt.Key_Down: {
                    if (symbian.listInteractionMode != Symbian.KeyNavigation) {
                        symbian.listInteractionMode = Symbian.KeyNavigation
                        internal.state = "Focused"
                        if (!flickableMode)
                            ListView.view.positionViewAtIndex(index, ListView.Beginning)
                    } else
                        if (!flickableMode)
                            ListView.view.incrementCurrentIndex()
                    event.accepted = !flickableMode
                    break
                }

                default: {
                    event.accepted = false
                    break
                }
            }

            root.keyPressed(event.key);
        }

        if ((event.key == Qt.Key_Up || event.key == Qt.Key_Down) && (!flickableMode))
            symbian.privateListItemKeyNavigation(ListView.view)
    }

    SequentialAnimation {
        id: releasedEffect
        PropertyAnimation {
            target: highlight
            property: "opacity"
            to: 0
            easing.type: Easing.Linear
            duration: 150
        }
    }

    Item {
        // non-visible item to create a padding boundary that content items can bind to
        id: paddingItem
        anchors {
            fill: parent
            leftMargin: platformStyle.paddingLarge
            rightMargin: iconLoader.status == Loader.Ready ?
                    privateStyle.scrollBarThickness + iconLoader.width + platformStyle.paddingMedium :
                    privateStyle.scrollBarThickness
            topMargin: platformStyle.paddingLarge
            bottomMargin: platformStyle.paddingLarge
        }
    }

    StateGroup {
        id: internal

        function getMode() {
            if (internal.state == "Pressed" || internal.state == "PressAndHold")
                return "pressed"
            else if (internal.state == "Focused")
                return "highlighted"
            else if (internal.state == "Disabled")
                return "disabled"
            else
                return "normal"
        }

        // Performance optimization:
        // Use value assignment when property changes instead of binding to js function
        onStateChanged: { root.mode = internal.getMode() }

        function press() {
            privateStyle.play(Symbian.BasicItem)
            highlight.source = privateStyle.imagePath("qtg_fr_list_pressed", root.platformInverted)
            highlight.opacity = 1
            if (root.ListView.view)
                root.ListView.view.currentIndex = index
        }

        function release() {
            if (symbian.listInteractionMode != Symbian.KeyNavigation)
                privateStyle.play(Symbian.BasicItem)
            releasedEffect.restart()
        }

        function releaseHold() {
            releasedEffect.restart()
        }

        function hold() {
            root.pressAndHold()
        }

        function disable() {
            faderLoader.opacity = 1
        }

        function focus() {
            faderLoader.opacity = 1
        }

        function canceled() {
            releasedEffect.restart()
        }

        states: [
            State { name: "Pressed" },
            State { name: "PressAndHold" },
            State { name: "Disabled"; when: !root.enabled },
            State { name: "Focused"; when: ((root.ListView.isCurrentItem || (flickableMode && activeFocus)) &&
                symbian.listInteractionMode == Symbian.KeyNavigation) },
            State { name: "Canceled" },
            State { name: "" }
        ]

        transitions: [
            Transition {
                to: "Pressed"
                ScriptAction { script: internal.press() }
            },
            Transition {
                from: "Pressed"
                to: "PressAndHold"
                ScriptAction { script: internal.hold() }
            },
            Transition {
                from: "PressAndHold"
                to: ""
                ScriptAction { script: internal.releaseHold() }
            },
            Transition {
                to: ""
                ScriptAction { script: internal.release() }
            },
            Transition {
                to: "Disabled"
                ScriptAction { script: internal.disable() }
            },
            Transition {
                to: "Focused"
                ScriptAction { script: internal.focus() }
            },
            Transition {
                to: "Canceled"
                ScriptAction { script: internal.canceled() }
            }
        ]
    }
}
