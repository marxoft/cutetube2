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
import QYouTube 1.0 as QYouTube
import ".."

ValueDialog {
    id: root

    titleText: qsTr("Video format")
    model: YouTubeStreamModel {
        id: streamModel

        onStatusChanged: {
            switch (status) {
            case QYouTube.StreamsRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QYouTube.StreamsRequest.Ready:
                if (count > 0) {
                    root.selectedIndex = Math.max(0, match("name", Settings.defaultPlaybackFormat(Resources.YOUTUBE)));
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case QYouTube.StreamsRequest.Failed:
                infoBanner.showMessage(errorString);
                break;
            default:
                break;
            }

            root.showProgressIndicator = false;
        }
    }
    onStatusChanged: if ((status == DialogStatus.Closing)
                         && (streamModel.status == QYouTube.StreamsRequest.Loading)) streamModel.cancel();
    
    onAccepted: Settings.setDefaultPlaybackFormat(Resources.YOUTUBE, streamModel.data(selectedIndex, "name"))
}
