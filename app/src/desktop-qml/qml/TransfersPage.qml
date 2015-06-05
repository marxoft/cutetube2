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
import QtQml 2.2
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import cuteTube 2.0

Page {
    id: root
    
    title: qsTr("Transfers")
    tools: ToolBarLayout {
        
        Label {
            id: label
            
            Layout.fillWidth: true
            text: root.title
        }
        
        Label {
            text: qsTr("Maximum concurrent transfers") + ":"
        }
        
        ComboBox {
            id: concurrentSelector
            
            model: ConcurrentTransfersModel {
                id: concurrentModel
            }
            textRole: "name"
            currentIndex: concurrentModel.match("value", Settings.maximumConcurrentTransfers)
            onActivated: Settings.maximumConcurrentTransfers = concurrentModel.data(index, "value")
        }
        
        ToolButton {
            id: startButton
        
            text: qsTr("Start")
            tooltip: qsTr("Start all transfers")
            iconName: "media-playback-start"
            enabled: Transfers.count > 0
            onClicked: Transfers.start()
        }
        
        ToolButton {
            id: pauseButton
        
            text: qsTr("Pause")
            tooltip: qsTr("Pause all transfers")
            iconName: "media-playback-pause"
            enabled: Transfers.count > 0
            onClicked: Transfers.pause()
        }        
    }
    
    TableView {
        id: view
        
        anchors.fill: parent
        focus: true
        model: TransferModel {
            id: transferModel
        }
        itemDelegate: Label {
            verticalAlignment: Text.AlignVCenter
            color: styleData.textColor
            elide: styleData.elideMode
            text: styleData.column == 3 ? Utils.formatBytes(transferModel.data(styleData.row, "bytesTransferred"))
                                          + " " + qsTr("of") + " "
                                          + Utils.formatBytes(transferModel.data(styleData.row, "size"))
                                          + " (" + styleData.value + "%)"
                                        : styleData.value
            
            MouseArea {
                anchors.fill: parent
                acceptedButtons: Qt.LeftButton | Qt.RightButton
                onPressed: {
                    view.forceActiveFocus();
                    view.selection.clear();
                    view.selection.select(styleData.row);
                    view.currentRow = styleData.row;
                }
                onClicked: if (mouse.button == Qt.RightButton) contextMenu.popup();
            }
        }
        
        TableViewColumn {
            role: "title"
            title: qsTr("Title")
            width: Math.floor(root.width / 3)
        }
        
        TableViewColumn {
            role: "category"
            title: qsTr("Category")
        }
        
        TableViewColumn {
            role: "priorityString"
            title: qsTr("Priority")
        }
        
        TableViewColumn {
            role: "progress"
            title: qsTr("Progress")
        }
        
        TableViewColumn {
            role: "statusString"
            title: qsTr("Status")
        }
    }
    
    Menu {
        id: contextMenu
        
        MenuItem {
            text: qsTr("Start")
            iconName: "media-playback-start"
            enabled: transferModel.data(view.currentRow, "status") <= Transfer.Failed
            onTriggered: Transfers.get(view.currentRow).queue()
        }
        
        MenuItem {
            text: qsTr("Pause")
            iconName: "media-playback-pause"
            enabled: transferModel.data(view.currentRow, "status") >= Transfer.Downloading
            onTriggered: Transfers.get(view.currentRow).pause()
        }
        
        Menu {
            id: categoryMenu
            
            title: qsTr("Category")
            visible: categoryModel.count > 0
            
            ExclusiveGroup {
                id: categoryGroup
            }
                            
            Instantiator {
                model: CategoryNameModel {
                    id: categoryModel
                }
                delegate: MenuItem {
                    text: name
                    checkable: true
                    exclusiveGroup: categoryGroup
                    checked: transferModel.data(view.currentRow, "category") == name
                    onTriggered: transferModel.setData(view.currentRow, name, "category")
                }
                onObjectAdded: categoryMenu.insertItem(index, object)
                onObjectRemoved: categoryMenu.removeItem(object)
            }
        }
        
        Menu {
            title: qsTr("Priority")
            
            ExclusiveGroup {
                id: priorityGroup
            }
            
            MenuItem {
                text: qsTr("High")
                checkable: true
                exclusiveGroup: priorityGroup
                checked: transferModel.data(view.currentRow, "priority") == Transfer.HighPriority
                onTriggered: transferModel.setData(view.currentRow, Transfer.HighPriority, "priority")
            }
            
            MenuItem {
                text: qsTr("Normal")
                checkable: true
                exclusiveGroup: priorityGroup
                checked: transferModel.data(view.currentRow, "priority") == Transfer.NormalPriority
                onTriggered: transferModel.setData(view.currentRow, Transfer.NormalPriority, "priority")
            }
            
            MenuItem {
                text: qsTr("Low")
                checkable: true
                exclusiveGroup: priorityGroup
                checked: transferModel.data(view.currentRow, "priority") == Transfer.LowPriority
                onTriggered: transferModel.setData(view.currentRow, Transfer.LowPriority, "priority")
            }
        }
    
        MenuItem {
            iconName: "edit-delete"
            text: qsTr("Remove")
            onTriggered: Transfers.get(view.currentRow).cancel()
        }
    }
}
