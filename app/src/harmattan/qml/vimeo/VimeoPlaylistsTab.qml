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
import com.nokia.meego 1.0
import cuteTube 2.0
import QVimeo 1.0 as QVimeo
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Tab {
    id: root

    property alias model: playlistModel

    title: qsTr("Albums")
    showProgressIndicator: playlistModel.status == QVimeo.ResourcesRequest.Loading
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-refresh"
            enabled: playlistModel.status != QVimeo.ResourcesRequest.Loading
            onClicked: playlistModel.reload()
        }
    }

    ListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        highlightFollowsCurrentItem: false
        model: VimeoPlaylistModel {
            id: playlistModel

            onStatusChanged: if (status == QVimeo.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: PlaylistDelegate {
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("VimeoPlaylistPage.qml")).load(playlistModel.get(index))
        }
    }

    ScrollDecorator {
        flickableItem: view
    }

    Label {
        anchors {
            fill: parent
            margins: UI.PADDING_DOUBLE
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 60
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
        text: qsTr("No albums found")
        visible: (playlistModel.status >= QVimeo.ResourcesRequest.Ready) && (playlistModel.count == 0)
    }
}
