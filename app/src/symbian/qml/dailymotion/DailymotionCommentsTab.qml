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
import cuteTube 2.0
import QDailymotion 1.0 as QDailymotion
import ".."

Tab {
    id: root

    property alias model: commentModel

    title: qsTr("Comments")
    showProgressIndicator: commentModel.status == QDailymotion.ResourcesRequest.Loading
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "../images/new-message.png"
            toolTipText: qsTr("Add comment")
            enabled: (Dailymotion.userId) && (Dailymotion.hasScope(Dailymotion.MANAGE_COMMENTS_SCOPE))
            onClicked: {
                dialogLoader.sourceComponent = commentDialog;
                dialogLoader.item.videoId = video.id;
                dialogLoader.item.open();
            }
        }

        MyToolButton {
            iconSource: "toolbar-refresh"
            toolTipText: qsTr("Reload")
            enabled: commentModel.status != QDailymotion.ResourcesRequest.Loading
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
        model: DailymotionCommentModel {
            id: commentModel

            onStatusChanged: if (status == QDailymotion.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: CommentDelegate {
            onThumbnailClicked: appWindow.pageStack.push(Qt.resolvedUrl("DailymotionUserPage.qml")).load(userId)
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
        visible: (commentModel.status >= QDailymotion.ResourcesRequest.Ready) && (commentModel.count == 0)
    }

    Loader {
        id: dialogLoader
    }

    Component {
        id: commentDialog

        DailymotionCommentDialog {
            focusItem: view
        }
    }
}
