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
import com.nokia.meego 1.0

SelectionDialog {
    id: root

    property bool showProgressIndicator: false

    title: Item {
        id: header

        anchors.fill: parent

        Item {
            id: labelField

            anchors.fill:  parent

            Item {
                id: labelWrapper

                height: titleLabel.height
                anchors {
                    left: labelField.left
                    right: progressIndicator.visible ? progressIndicator.left : closeButton.left
                    bottom:  parent.bottom
                    bottomMargin: root.platformStyle.titleBarLineMargin
                }

                Label {
                    id: titleLabel

                    anchors {
                        left: parent.left
                        leftMargin: root.platformStyle.titleBarIndent
                        right: parent.right
                    }
                    font: root.platformStyle.titleBarFont
                    color: root.platformStyle.commonLabelColor
                    elide: root.platformStyle.titleElideMode
                    text: root.titleText
                }
            }

            BusyIndicator {
                id: progressIndicator

                anchors {
                    right: closeButton.left
                    rightMargin: root.platformStyle.titleBarLineMargin
                    bottom: parent.bottom
                    bottomMargin: root.platformStyle.titleBarLineMargin
                }
                running: visible
                visible: root.showProgressIndicator
                platformStyle: BusyIndicatorStyle {
                    inverted: true
                }
            }

            Image {
                id: closeButton

                anchors {
                    right: labelField.right
                    bottom:  parent.bottom
                    bottomMargin: root.platformStyle.titleBarLineMargin - 6
                }
                opacity: closeButtonArea.pressed ? 0.5 : 1.0
                source: "image://theme/icon-m-common-dialog-close"

                MouseArea {
                    id: closeButtonArea
                    anchors.fill: parent
                    onClicked: root.reject()
                }

            }
        }

        Rectangle {
            id: headerLine

            height: 1
            anchors {
                left: parent.left
                right: parent.right
                bottom: header.bottom
            }
            color: "#4d4d4d"
        }
    }
}
