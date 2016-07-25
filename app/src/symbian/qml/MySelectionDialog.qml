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

Dialog {
    id: root

    property Item focusItem: null
    property int selectedIndex: -1
    property bool showProgressIndicator: false
    property Component delegate: defaultDelegate
    property string titleText
    property alias model: listView.model

    title: Item {
        anchors.left: parent.left
        anchors.right: parent.right
        height: platformStyle.graphicSizeSmall + 2 * platformStyle.paddingLarge

        LayoutMirroring.enabled: false
        LayoutMirroring.childrenInherit: true

        Item {
            id: titleLayoutHelper // needed to make the text mirror correctly

            anchors.left: parent.left
            anchors.right: progressIndicator.visible ? progressIndicator.left : titleAreaIcon.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.margins: platformStyle.paddingLarge

            Text {
                id: titleAreaText

                LayoutMirroring.enabled: root.LayoutMirroring.enabled

                anchors.fill: parent
                font { family: platformStyle.fontFamilyRegular; pixelSize: platformStyle.fontSizeLarge }
                color: root.platformInverted ? platformStyle.colorNormalLinkInverted
                                             : platformStyle.colorNormalLink
                elide: Text.ElideRight
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignVCenter
                text: root.titleText
            }
        }

        BusyIndicator {
            id: progressIndicator

            width: platformStyle.graphicSizeSmall
            height: platformStyle.graphicSizeSmall
            anchors.right: titleAreaIcon.left
            anchors.verticalCenter: parent.verticalCenter
            running: visible
            visible: root.showProgressIndicator
        }

        Image {
            id: titleAreaIcon

            anchors.right: parent.right
            anchors.rightMargin: platformStyle.paddingLarge
            anchors.verticalCenter: parent.verticalCenter
            source: privateStyle.imagePath((iconMouseArea.pressed && !iconMouseArea.pressCancelled
                                            ? "qtg_graf_popup_close_pressed"
                                            : "qtg_graf_popup_close_normal"),
                                           root.platformInverted)
            sourceSize.height: platformStyle.graphicSizeSmall
            sourceSize.width: platformStyle.graphicSizeSmall

            MouseArea {
                id: iconMouseArea

                property bool pressCancelled

                anchors.centerIn: parent
                width: parent.width + 2 * platformStyle.paddingLarge
                height: parent.height + 2 * platformStyle.paddingLarge
                enabled: root.status == DialogStatus.Open

                onPressed: {
                    pressCancelled = false
                    privateStyle.play(Symbian.BasicButton)
                }
                onClicked: {
                    if (!pressCancelled)
                        root.reject()
                }
                onReleased: {
                    if (!pressCancelled)
                        privateStyle.play(Symbian.PopupClose)
                }
                onExited: pressCancelled = true
            }
        }
    }

    content: Item {
        id: contentItem

        function preferredHeight() {
            // Need to create artifical binding to listView.delegate because of QTBUG-19037
            var multiplier = screen.height > screen.width ? 0.45 : 0.6;
            var maxHeight = Math.floor(multiplier * screen.height / privateStyle.menuItemHeight)
                    * privateStyle.menuItemHeight;
            return maxHeight;
        }

        height: preferredHeight()
        width: root.platformContentMaximumWidth

        Item {
            // Clipping item with bottom margin added to align content with rounded background graphics
            id: clipItem
            anchors.fill: parent
            anchors.bottomMargin: platformStyle.paddingSmall
            clip: true
            ListView {
                id: listView

                currentIndex : -1
                width: contentItem.width
                height: contentItem.height
                delegate: root.delegate
                clip: true

                Keys.onPressed: {
                    if (event.key == Qt.Key_Up || event.key == Qt.Key_Down
                            || event.key == Qt.Key_Left || event.key == Qt.Key_Right
                            || event.key == Qt.Key_Select || event.key == Qt.Key_Enter
                            || event.key == Qt.Key_Return) {
                        symbian.listInteractionMode = Symbian.KeyNavigation
                        listView.currentIndex = 0
                        event.accepted = true
                    }
                }
            }
        }
        ScrollBar {
            id: scrollBar
            flickableItem: listView
            interactive: false
            visible: listView.contentHeight > contentItem.height
            platformInverted: root.platformInverted
            anchors { top: clipItem.top; right: clipItem.right }
        }
    }

    Component {
        id: defaultDelegate

        MenuItem {
            text: name
            onClicked: {
                root.selectedIndex = index;
                root.accept();
            }

            Keys.onPressed: {
                if (event.key == Qt.Key_Up || event.key == Qt.Key_Down)
                    scrollBar.flash()
            }
        }
    }

    onClickedOutside: reject()
    onStatusChanged: if ((status === DialogStatus.Closed) && (focusItem)) focusItem.forceActiveFocus();
}
