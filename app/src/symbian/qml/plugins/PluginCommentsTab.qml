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
import cuteTube 2.0
import ".."

Tab {
    id: root

    property alias model: commentModel

    title: qsTr("Comments")
    showProgressIndicator: commentModel.status == ResourcesRequest.Loading
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-refresh"
            toolTipText: qsTr("Reload")
            enabled: commentModel.status != ResourcesRequest.Loading
            onClicked: commentModel.reload()
        }
    }

    MyListView {
        id: view

        anchors {
            fill: parent
            topMargin: platformStyle.paddingLarge
        }
        cacheBuffer: 400
        model: PluginCommentModel {
            id: commentModel

            service: Settings.currentService
            onStatusChanged: if (status == ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: CommentDelegate {
            onThumbnailClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginUserPage.qml")).load(userId)
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
        text: qsTr("No comments found")
        visible: (commentModel.status >= ResourcesRequest.Ready) && (commentModel.count == 0)
    }
}
