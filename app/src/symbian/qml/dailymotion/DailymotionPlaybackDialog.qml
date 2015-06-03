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
import QDailymotion 1.0 as QDailymotion
import ".."

ValueDialog {
    id: root

    titleText: qsTr("Video format")
    model: DailymotionStreamModel {
        id: streamModel

        onStatusChanged: {
            switch (status) {
            case QDailymotion.StreamsRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case QDailymotion.StreamsRequest.Ready:
                if (count > 0) {
                    root.selectedIndex = Math.max(0, match("name", Settings.defaultPlaybackFormat(Resources.DAILYMOTION)));
                }
                else {
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case QDailymotion.StreamsRequest.Failed: {
                infoBanner.showMessage(errorString);
                break;
            }
            default:
                break;
            }

            root.showProgressIndicator = false;
        }
    }
    onStatusChanged: if ((status == DialogStatus.Closing)
                         && (streamModel.status == QDailymotion.StreamsRequest.Loading)) streamModel.cancel();
    
    onAccepted: Settings.setDefaultPlaybackFormat(Resources.DAILYMOTION, streamModel.data(selectedIndex, "name"))
}
