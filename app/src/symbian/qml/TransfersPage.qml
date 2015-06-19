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

import QtQuick 1.1
import com.nokia.symbian 1.1
import cuteTube 2.0

MyPage {
    id: root

    title: qsTr("Transfers")
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-mediacontrol-play"
            toolTipText: qsTr("Start")
            enabled: Transfers.count > 0
            onClicked: Transfers.start()
        }

        MyToolButton {
            iconSource: "toolbar-mediacontrol-pause"
            toolTipText: qsTr("Pause")
            enabled: Transfers.count > 0
            onClicked: Transfers.pause()
        }
    }

    MyListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        model: TransferModel {
            id: transferModel
        }
        delegate: TransferDelegate {
            id: delegate

            onClicked: {
                view.currentIndex = -1;
                view.currentIndex = index;
                contextMenu.open();
            }
            onPressAndHold: {
                view.currentIndex = -1;
                view.currentIndex = index;
                contextMenu.open();
            }
        }
    }

    MyScrollBar {
        flickableItem: view
    }

    Label {
        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 40
        color: platformStyle.colorNormalMid
        text: qsTr("No transfers")
        visible: Transfers.count == 0
    }

    MyContextMenu {
        id: contextMenu

        focusItem: view

        MenuLayout {

            MenuItem {
                text: qsTr("Start")
                enabled: transferModel.data(view.currentIndex, "status") <= Transfer.Queued
                onClicked: Transfers.get(view.currentIndex).queue()
            }

            MenuItem {
                text: qsTr("Pause")
                enabled: transferModel.data(view.currentIndex, "status") >= Transfer.Downloading
                onClicked: Transfers.get(view.currentIndex).pause()
            }

            ValueMenuItem {
                title: qsTr("Priority")
                subTitle: view.currentIndex >= 0 ? transferModel.data(view.currentIndex, "priorityString") : "-"
                onClicked: {
                    dialogLoader.sourceComponent = priorityDialog;
                    dialogLoader.item.open();
                }
            }

            ValueMenuItem {
                title: qsTr("Category")
                subTitle: view.currentIndex >= 0 ? transferModel.data(view.currentIndex, "category") : "-"
                onClicked: {
                    dialogLoader.sourceComponent = categoryDialog;
                    dialogLoader.item.open();
                }
            }

            MenuItem {
                text: qsTr("Remove")
                onClicked: Transfers.get(view.currentIndex).cancel()
            }
        }
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: priorityDialog

        ValueDialog {
            titleText: qsTr("Priority")
            focusItem: view
            model: TransferPriorityModel {}
            onAccepted: transferModel.setData(view.currentIndex, value, "priority")
        }
    }

    Component {
        id: categoryDialog

        ValueDialog {
            titleText: qsTr("Category")
            focusItem: view
            model: CategoryNameModel {}
            onAccepted: transferModel.setData(view.currentIndex, value, "category")
        }
    }
}
