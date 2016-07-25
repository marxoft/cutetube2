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
import cuteTube 2.0
import QDailymotion 1.0 as QDailymotion
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Tab {
    id: root

    property alias model: commentModel

    title: qsTr("Comments")
    showProgressIndicator: commentModel.status == QDailymotion.ResourcesRequest.Loading
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-new-message"
            enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_COMMENTS_SCOPE))
            onClicked: {
                dialogLoader.sourceComponent = commentDialog;
                dialogLoader.item.videoId = video.id;
                dialogLoader.item.open();
            }
        }

        MyToolIcon {
            platformIconId: "toolbar-refresh"
            enabled: commentModel.status != QDailymotion.ResourcesRequest.Loading
            onClicked: commentModel.reload()
        }
    }

    ListView {
        id: view

        anchors {
            fill: parent
            topMargin: UI.PADDING_DOUBLE
        }
        cacheBuffer: 400
        highlightFollowsCurrentItem: false
        model: DailymotionCommentModel {
            id: commentModel

            onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: CommentDelegate {
            onThumbnailClicked: appWindow.pageStack.push(Qt.resolvedUrl("DailymotionUserPage.qml")).load(userId)
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
        text: qsTr("No comments found")
        visible: (commentModel.status >= QDailymotion.ResourcesRequest.Ready) && (commentModel.count == 0)
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: commentDialog

        DailymotionCommentDialog {}
    }
}
