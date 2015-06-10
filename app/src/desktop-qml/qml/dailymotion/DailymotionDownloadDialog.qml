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
import QtQuick.Layouts 1.1
import cuteTube 2.0
import QDailymotion 1.0 as QDailymotion
import ".."

MyDialog {
    id: root
        
    property string resourceId
    property string resourceTitle
        
    minimumWidth: grid.width + 20
    minimumHeight: grid.height + 60
    title: qsTr("Download video")
    content: GridLayout {
        id: grid
        
        columns: 2
        columnSpacing: 10
        rowSpacing: 10
        
        Label {
            text: qsTr("Video format") + ":"
        }
        
        ComboBox {
            id: streamSelector
        
            Layout.minimumWidth: 200
            model: DailymotionStreamModel {
                id: streamModel
            
                onStatusChanged: {
                    switch (status) {
                    case QDailymotion.StreamsRequest.Ready:
                        if (count > 0) {
                            streamSelector.currentIndex = Math.max(0, match("name",
                                                                   Settings.defaultDownloadFormat(Resources.DAILYMOTION)));
                        }
                        else {
                            messageBox.showError(qsTr("No streams found"));
                        }

                        break;
                    case QDailymotion.StreamsRequest.Failed: {
                        messageBox.showError(errorString);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            textRole: "name"
            onActivated: Settings.setDefaultDownloadFormat(Resources.DAILYMOTION, streamModel.data(index, "name"))
        }
        
        CheckBox {
            id: audioCheckBox
            
            Layout.columnSpan: 2
            text: qsTr("Convert to audio file")
            enabled: AUDIO_CONVERTOR_ENABLED
        }
        
        CheckBox {
            id: subtitleCheckBox
            
            Layout.columnSpan: 2
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
        
        Label {
            text: qsTr("Subtitles language") + ":"
        }
        
        ComboBox {
            id: subtitleSelector
        
            Layout.minimumWidth: 200
            model: DailymotionSubtitleModel {
                id: subtitleModel
            
                onStatusChanged: {
                    switch (status) {
                    case QDailymotion.ResourcesRequest.Ready:
                        if (count > 0) {
                            subtitleSelector.currentIndex = Math.max(0, match("name", Settings.subtitlesLanguage));
                        }
                        else {
                            subtitleCheckBox.checked = false;
                            subtitleCheckBox.enabled = false;
                            messageBox.showError(qsTr("No subtitles found"));
                        }

                        break;
                    case QDailymotion.ResourcesRequest.Failed: {
                        messageBox.showError(errorString);
                        break;
                    }
                    default:
                        break;
                    }
                }
            }
            textRole: "name"
            enabled: subtitleCheckBox.checked
            onActivated: Settings.subtitlesLanguage = subtitleModel.data(index, "name")
        }
        
        Label {
            text: qsTr("Category") + ":"
        }
        
        ComboBox {
            id: categorySelector
            
            Layout.minimumWidth: 200
            model: CategoryNameModel {
                id: categoryModel
            }
            textRole: "name"
            currentIndex: categoryModel.match("value", Settings.defaultCategory)
            onActivated: Settings.defaultCategory = categoryModel.data(index, "value")
        }
    }
    buttons: [
        Button {
            text: qsTr("&Cancel")
            iconName: "dialog-cancel"
            onClicked: root.reject()
        },
        
        Button {
            text: qsTr("&Ok")
            iconName: "dialog-ok"
            isDefault: true
            enabled: streamModel.count > 0
            onClicked: root.accept()
        }
    ]
    
    onOpened: {
        subtitleCheckBox.checked = false;
        subtitleModel.clear();
        streamModel.list(resourceId);
    }
    onClosed: {
        streamModel.cancel();
        subtitleModel.cancel();
    }
    onAccepted: Transfers.addDownloadTransfer(Resources.DAILYMOTION, resourceId,
                                              streamModel.data(streamSelector.currentIndex, "value").id, "",
                                              resourceTitle, categoryModel.data(categorySelector.currentIndex, "value"),
                                              subtitleCheckBox.checked ? subtitleModel.data(subtitleSelector.currentIndex, "name") : "",
                                              audioCheckBox.checked)
}
