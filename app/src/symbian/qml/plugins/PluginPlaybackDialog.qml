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
import ".."

ValueDialog {
    id: root

    titleText: qsTr("Video format")
    model: PluginStreamModel {
        id: streamModel

        service: Settings.currentService
        onStatusChanged: {
            switch (status) {
            case ResourcesRequest.Loading:
                root.showProgressIndicator = true;
                return;
            case ResourcesRequest.Ready:
                if (count > 0) {
                    root.selectedIndex = Math.max(0, match("name", Settings.defaultPlaybackFormat(service)));
                }
                else {
                    root.reject();
                    infoBanner.showMessage(qsTr("No streams found"));
                }

                break;
            case ResourcesRequest.Failed: {
                root.reject();
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
                         && (streamModel.status == ResourcesRequest.Loading)) streamModel.cancel();
    
    onAccepted: Settings.setDefaultPlaybackFormat(streamModel.service, streamModel.data(selectedIndex, "name"))
}
