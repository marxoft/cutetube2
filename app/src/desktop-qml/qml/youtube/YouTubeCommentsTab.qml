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
import QYouTube 1.0 as QYouTube
import ".."

Tab {
    id: root
        
    property variant model: item ? item.model : null
    property variant view: item ? item.view : null
    
    ScrollView {
        id: content
        
        property alias model: commentModel
        property alias view: view
        
        width: parent ? parent.width : undefined
        height: parent ? parent.height : undefined
        focus: true
    
        ListView {
            id: view
        
            anchors.fill: parent
            model: YouTubeCommentModel {
                id: commentModel
                
                onStatusChanged: if (status == QYouTube.ResourcesRequest.Failed) messageBox.showError(errorString);
            }
            delegate: CommentDelegate {
                onThumbnailClicked: pageStack.push({item: Qt.resolvedUrl("YouTubeUserPage.qml"), immediate: true}).load(userId)
            }
        }
    }
}
