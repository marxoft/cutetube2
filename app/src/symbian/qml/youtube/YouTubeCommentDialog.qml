/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1
import cuteTube 2.0
import QYouTube 1.0 as QYouTube
import ".."

MySheet {
    id: root

    property string videoId
    property string parentId

    acceptWhenDone: false
    showProgressIndicator: comment.status == QYouTube.ResourcesRequest.Loading
    rejectButtonText: qsTr("Cancel")
    acceptButtonText: commentField.text ? qsTr("Done") : ""
    content: Item {
        id: contentItem

        anchors.fill: parent
        clip: true

        KeyNavFlickable {
            id: flicker

            anchors.fill: parent
            contentHeight: column.height + platformStyle.paddingLarge

            Column {
                id: column

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: platformStyle.paddingLarge
                }
                spacing: platformStyle.paddingLarge

                Label {
                    font.bold: true
                    text: qsTr("Comment")
                }

                TextArea {
                    id: commentField

                    width: parent.width
                }
            }
        }

        MyScrollBar {
            flickableItem: flicker
        }
    }

    YouTubeComment {
        id: comment

        onStatusChanged: {
            switch (status) {
            case QYouTube.ResourcesRequest.Ready:
                root.accept();
                break;
            case QYouTube.ResourcesRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }
        }
    }

    onDone: {
        var c = {};
        var s = {};
        s["videoId"] = root.videoId;
        s["textDisplay"] = commentField.text;

        if (root.parentId) {
            s["parentId"] = root.parentId;
        }

        c["snippet"] = s;
        comment.addComment(c);
    }

    onStatusChanged: {
        if (status == DialogStatus.Closed) {
            videoId = "";
            parentId = "";
            commentField.text = "";
        }
    }
}
