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
import QYouTube 1.0 as QYouTube
import ".."

MyPage {
    id: root

    property alias model: categoryModel

    title: qsTr("Categories")
    showProgressIndicator: categoryModel.status == QYouTube.ResourcesRequest.Loading
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-refresh"
            toolTipText: qsTr("Reload")
            enabled: categoryModel.status != QYouTube.ResourcesRequest.Loading
            onClicked: categoryModel.reload()
        }
    }

    MyListView {
        id: view

        anchors.fill: parent
        cacheBuffer: 400
        model: YouTubeCategoryModel {
            id: categoryModel

            onStatusChanged: if (status == QYouTube.ResourcesRequest.Failed) infoBanner.showMessage(errorString);
        }
        delegate: LabelDelegate {
            text: name
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("YouTubeVideosPage.qml"), {title: name})
            .model.list("/videos", ["snippet", "contentDetails", "statistics"], {chart: "mostPopular"},
                        {videoCategoryId: value, regionCode: Qt.locale().name.split("_")[1], maxResults: MAX_RESULTS})
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
        text: qsTr("No categories found")
        visible: (categoryModel.status >= QYouTube.ResourcesRequest.Ready) && (categoryModel.count == 0)
    }
}
