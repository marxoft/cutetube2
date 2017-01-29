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

MySheet {
    id: root

    property string videoId
    property string videoTitle
    property string streamUrl
    property bool listSubtitles

    showProgressIndicator: (streamModel.status == ResourcesRequest.Loading)
                           || (subtitleModel.status == ResourcesRequest.Loading)
    acceptButtonText: streamModel.count > 0 ? qsTr("Done") : ""
    rejectButtonText: qsTr("Cancel")
    content: Item {
        anchors.fill: parent

        MyFlickable {
            id: flicker

            anchors.fill: parent
            contentHeight: column.height + platformStyle.paddingLarge

            Column {
                id: column

                anchors {
                    left: parent.left
                    right: parent.right
                    top: parent.top
                }

                ValueSelector {
                    id: streamSelector

                    width: parent.width
                    title: qsTr("Video format")
                    focusItem: flicker
                    model: PluginStreamModel {
                        id: streamModel

                        service: Settings.currentService
                        onStatusChanged: {
                            switch (status) {
                            case ResourcesRequest.Loading:
                                streamSelector.showProgressIndicator = true;
                                return;
                            case ResourcesRequest.Ready:
                                if (count > 0) {
                                    streamSelector.selectedIndex = Math.max(0, match("name",
                                                                            Settings.defaultDownloadFormat(service)));
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

                            streamSelector.showProgressIndicator = false;
                        }
                    }
                    onAccepted: Settings.setDefaultDownloadFormat(streamModel.service,
                                                                  streamModel.data(selectedIndex, "name"))
                }
                
                ValueSelector {
                    id: categorySelector

                    width: parent.width
                    title: qsTr("Category")
                    focusItem: flicker
                    model: CategoryNameModel {
                        id: categoryModel
                    }
                    value: Settings.defaultCategory
                    onValueChanged: Settings.defaultCategory = value
                }
                
                MySwitch {
                    id: subtitleSwitch
            
                    text: qsTr("Download subtitles")
                    onCheckedChanged: {
                        if (checked) {
                            if (subtitleModel.status != ResourcesRequest.Loading) {
                                subtitleModel.list(root.videoId);
                            }
                        }
                        else {
                            subtitleModel.cancel();
                        }
                    }
                }
                
                ValueSelector {
                    id: subtitleSelector

                    width: parent.width
                    title: qsTr("Subtitles language")
                    focusItem: flicker
                    enabled: subtitleSwitch.checked
                    model: PluginSubtitleModel {
                        id: subtitleModel

                        service: Settings.currentService
                        onStatusChanged: {
                            switch (status) {
                            case ResourcesRequest.Loading:
                                subtitleSelector.showProgressIndicator = true;
                                return;
                            case ResourcesRequest.Ready:
                                if (count > 0) {
                                    subtitleSelector.selectedIndex = Math.max(0, match("name",
                                                                                       Settings.subtitlesLanguage));
                                }
                                else {
                                    infoBanner.showMessage(qsTr("No subtitles found"));
                                }

                                break;
                            case ResourcesRequest.Failed: {
                                subtitleSwitch.checked = false;
                                subtitleSwitch.enabled = false;
                                infoBanner.showMessage(errorString);
                                break;
                            }
                            default:
                                break;
                            }

                            subtitleSelector.showProgressIndicator = false;
                        }
                    }
                    onAccepted: Settings.setDefaultDownloadFormat(streamModel.service,
                                                                  streamModel.data(selectedIndex, "name"))
                }                
            }
        }

        MyScrollBar {
            flickableItem: flicker
        }
    }

    onAccepted: Transfers.addDownloadTransfer(streamModel.service, videoId, streamUrl ? "" : streamSelector.value.id,
                                              streamUrl, videoTitle, Settings.defaultCategory,
                                              subtitleSwitch.checked ?
                                              subtitleModel.data(subtitleSelector.selectedIndex, "name") : "")

    onStatusChanged: {
        switch (status) {
        case DialogStatus.Open: {
            streamModel.clear();
            streamSelector.selectedIndex = -1;
            subtitleModel.clear();
            subtitleSelector.selectedIndex = -1;
            subtitleSwitch.checked = false;
            subtitleSwitch.enabled = listSubtitles;
            
            if (streamUrl) {
                streamModel.append(qsTr("Default format"), streamUrl);
                streamSelector.selectedIndex = 0;
            }
            else {
                streamModel.list(videoId);
            }
            
            break;
        }
        case DialogStatus.Closed: {
            streamModel.cancel();
            subtitleModel.cancel();
            break;
        }
        default:
            break;
        }
    }
}
