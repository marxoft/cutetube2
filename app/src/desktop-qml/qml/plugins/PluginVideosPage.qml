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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import cuteTube 2.0
import ".."

Page {
    id: root
        
    property alias model: videoModel
    property alias view: view
    
    title: qsTr("Videos")
    tools: ToolBarLayout {
        
        ToolButton {
            id: backButton
            
            text: qsTr("Back")
            tooltip: qsTr("Go back")
            iconName: "go-previous"
            visible: root.Stack.index > 0
            onClicked: pageStack.pop({immediate: true})
        }
        
        Label {
            id: label
            
            Layout.fillWidth: true
            text: root.title
        }
        
        ToolButton {
            id: reloadButton
        
            text: qsTr("Reload")
            tooltip: qsTr("Reload")
            iconName: "view-refresh"
            enabled: videoModel.status != ResourcesRequest.Loading
            onClicked: videoModel.reload()
        }
        
        ViewModeButton {
            id: viewButton
        }
    }
    
    PluginVideo {
        id: video
    }
    
    ItemView {
        id: view
        
        anchors {
            fill: parent
            leftMargin: mode == "grid" ? 10 : 0
        }
        mode: Settings.defaultViewMode
        model: PluginVideoModel {
            id: videoModel
            
            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
        }
        delegate: VideoDelegate {
            mode: view.mode
            onClicked: pageStack.push({item: Qt.resolvedUrl("PluginVideoPage.qml"), immediate: true})
                                     .load(videoModel.get(index))
            onRightClicked: contextMenu.popup()
        }
    }
    
    Menu {
        id: contextMenu
    
        MenuItem {
            text: qsTr("Download")
            iconName: "folder-download"
            onTriggered: {
                loader.sourceComponent = downloadDialog;
                loader.item.list(videoModel.data(view.currentIndex, "id"), videoModel.data(view.currentIndex, "title"));
                loader.item.open();
            }
        }
        
        MenuItem {
            text: qsTr("Copy URL")
            iconName: "applications-internet"
            onTriggered: {
                Clipboard.text = videoModel.data(view.currentIndex, "url");
                statusBar.showMessage(qsTr("URL copied to clipboard"));
            }
        }           
    }
    
    Loader {
        id: loader
    }
    
    Component {
        id: downloadDialog
        
        PluginDownloadDialog {}
    }
}
