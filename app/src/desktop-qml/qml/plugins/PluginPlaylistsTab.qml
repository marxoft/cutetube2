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
import cuteTube 2.0
import ".."

Tab {
    id: root
        
    property variant model: item ? item.model : null
    property variant view: item ? item.view : null
    
    title: qsTr("Playlists")
    
    FocusScope {
        id: content
        
        property alias model: playlistModel
        property alias view: view
        
        width: parent ? parent.width : undefined
        height: parent ? parent.height : undefined
        focus: true
    
        ItemView {
            id: view
        
            anchors {
                fill: parent
                leftMargin: mode == "grid" ? 10 : 0
            }
            mode: Settings.defaultViewMode
            model: PluginPlaylistModel {
                id: playlistModel
                
                service: Settings.currentService
                onStatusChanged: if (status == ResourcesRequest.Failed) messageBox.showError(errorString);
            }
            delegate: PlaylistDelegate {
                mode: view.mode
                onClicked: pageStack.push({item: Qt.resolvedUrl("PluginPlaylistPage.qml"), immediate: true})
                                         .load(playlistModel.get(index))
            }
        }
    }
}
