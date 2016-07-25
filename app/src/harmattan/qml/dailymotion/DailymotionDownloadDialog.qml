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

MySheet {
    id: root

    property string resourceId
    property string resourceTitle

    showProgressIndicator: (streamModel.status == QDailymotion.StreamsRequest.Loading)
                           || (subtitleModel.status == QDailymotion.ResourcesRequest.Loading)
    acceptButtonText: streamModel.count > 0 ? qsTr("Done") : ""
    rejectButtonText: qsTr("Cancel")
    content: Item {
        anchors.fill: parent

        Flickable {
            id: flicker

            anchors.fill: parent
            contentHeight: column.height + UI.PADDING_DOUBLE

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
                    model: DailymotionStreamModel {
                        id: streamModel

                        onStatusChanged: {
                            switch (status) {
                            case QDailymotion.StreamsRequest.Loading:
                                streamSelector.showProgressIndicator = true;
                                return;
                            case QDailymotion.StreamsRequest.Ready:
                                if (count > 0) {
                                    streamSelector.selectedIndex = Math.max(0, match("name",
                                                                            Settings.defaultDownloadFormat(Resources.DAILYMOTION)));
                                }
                                else {
                                    infoBanner.showMessage(qsTr("No streams found"));
                                }

                                break;
                            case QDailymotion.StreamsRequest.Failed:
                                infoBanner.showMessage(errorString);
                                break;
                            default:
                                break;
                            }

                            streamSelector.showProgressIndicator = false;
                        }
                    }
                    onValueChanged: Settings.setDefaultDownloadFormat(Resources.DAILYMOTION, subTitle)
                }

                ValueSelector {
                    id: categorySelector

                    width: parent.width
                    title: qsTr("Category")
                    model: CategoryNameModel {
                        id: categoryModel
                    }
                    value: Settings.defaultCategory
                    onValueChanged: Settings.defaultCategory = value
                }
                
                Item {
                    width: parent.width
                    height: UI.PADDING_DOUBLE
                }

                MySwitch {
                    id: subtitleSwitch

                    text: qsTr("Download subtitles")
                    onCheckedChanged: {
                        if (checked) {
                            if (subtitleModel.status != QDailymotion.ResourcesRequest.Loading) {
                                subtitleModel.list(root.resourceId);
                            }
                        }
                        else {
                            subtitleModel.cancel();
                        }
                    }
                }
                
                Item {
                    width: parent.width
                    height: UI.PADDING_DOUBLE
                }

                ValueSelector {
                    id: subtitleSelector

                    width: parent.width
                    title: qsTr("Subtitles language")
                    model: DailymotionSubtitleModel {
                        id: subtitleModel

                        onStatusChanged: {
                            switch (status) {
                            case QDailymotion.ResourcesRequest.Loading:
                                subtitleSelector.showProgressIndicator = true;
                                return;
                            case QDailymotion.ResourcesRequest.Ready:
                                if (count > 0) {
                                    subtitleSelector.selectedIndex = Math.max(0, match("name",
                                                                              Settings.subtitlesLanguage));
                                }
                                else {
                                    subtitleSwitch.checked = false;
                                    subtitleSwitch.enabled = false;
                                    infoBanner.showMessage(qsTr("No subtitles found"));
                                }

                                break;
                            case QDailymotion.ResourcesRequest.Failed: {
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
                    enabled: subtitleSwitch.checked
                    onSelectedIndexChanged: Settings.subtitlesLanguage = subtitleModel.data(selectedIndex, "name")
                }
                
                Item {
                    width: parent.width
                    height: UI.PADDING_DOUBLE
                }
                
                Label {
                    width: parent.width
                    font.bold: true
                    text: qsTr("Custom command (%f for filename)")
                }
                
                Item {
                    width: parent.width
                    height: UI.PADDING_DOUBLE
                }
                
                MyTextField {
                    id: commandField
                    
                    width: parent.width
                }
                
                Item {
                    width: parent.width
                    height: UI.PADDING_DOUBLE
                }
                
                MySwitch {
                    id: commandSwitch
                    
                    width: parent.width
                    text: qsTr("Override global custom command")
                }
            }
        }

        ScrollDecorator {
            flickableItem: flicker
        }
    }

    onAccepted: Transfers.addDownloadTransfer(Resources.DAILYMOTION, resourceId, streamSelector.value.id, "",
                                              resourceTitle, Settings.defaultCategory,
                                              subtitleSwitch.checked ? Settings.subtitlesLanguage : "",
                                              commandField.text, commandSwitch.checked)

    onStatusChanged: {
        switch (status) {
        case DialogStatus.Opening: {
            subtitleSwitch.checked = false;
            subtitleModel.clear();
            commandField.text = "";
            commandSwitch .checked = false;
            streamModel.list(resourceId);
            break;
        }
        case DialogStatus.Closing: {
            streamModel.cancel();
            subtitleModel.cancel();
            break;
        }
        default:
            break;
        }
    }
}
