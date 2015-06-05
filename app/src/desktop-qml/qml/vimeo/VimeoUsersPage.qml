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
import QVimeo 1.0 as QVimeo
import ".."

Page {
    id: root
        
    property alias model: userModel
    property alias view: view
    
    title: qsTr("Users")
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
            enabled: userModel.status != QVimeo.ResourcesRequest.Loading
            onClicked: userModel.reload()
        }
        
        ViewModeButton {
            id: viewButton
        }
    }
    
    ItemView {
        id: view
        
        anchors {
            fill: parent
            leftMargin: mode == "grid" ? 10 : 0
        }
        mode: Settings.defaultViewMode
        model: VimeoUserModel {
            id: userModel
            
            onStatusChanged: if (status == QVimeo.ResourcesRequest.Failed) messageBox.showError(errorString);
        }
        delegate: UserDelegate {
            mode: view.mode
            onClicked: pageStack.push({item: Qt.resolvedUrl("VimeoUserPage.qml"), immediate: true})
                                     .load(userModel.get(index))
        }
    }
}
