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
import QVimeo 1.0 as QVimeo
import ".."

MySheet {
    id: root

    property string resourceId
    property string resourceTitle

    showProgressIndicator: (streamModel.status == QVimeo.StreamsRequest.Loading)
                           || (subtitleModel.status == QVimeo.ResourcesRequest.Loading)
    acceptButtonText: streamModel.count > 0 ? qsTr("Done") : ""
    rejectButtonText: qsTr("Cancel")
    content: Item {
        anchors.fill: parent

        KeyNavFlickable {
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
                    model: VimeoStreamModel {
                        id: streamModel

                        onStatusChanged: {
                            switch (status) {
                            case QVimeo.StreamsRequest.Loading:
                                streamSelector.showProgressIndicator = true;
                                return;
                            case QVimeo.StreamsRequest.Ready:
                                if (count > 0) {
                                    streamSelector.selectedIndex = Math.max(0, match("name",
                                                                            Settings.defaultDownloadFormat(Resources.VIMEO)));
                                }
                                else {
                                    infoBanner.showMessage(qsTr("No streams found"));
                                }

                                break;
                            case QVimeo.StreamsRequest.Failed: {
                                infoBanner.showMessage(errorString);
                                break;
                            }
                            default:
                                break;
                            }

                            streamSelector.showProgressIndicator = false;
                        }
                    }
                    onAccepted: Settings.setDefaultDownloadFormat(Resources.VIMEO, streamModel.data(selectedIndex, "name"))
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
                            if (subtitleModel.status != QVimeo.ResourcesRequest.Loading) {
                                subtitleModel.list(root.resourceId);
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
                    model: VimeoSubtitleModel {
                        id: subtitleModel

                        onStatusChanged: {
                            switch (status) {
                            case QVimeo.ResourcesRequest.Loading:
                                subtitleSelector.showProgressIndicator = true;
                                return;
                            case QVimeo.ResourcesRequest.Ready:
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
                            case QVimeo.ResourcesRequest.Failed: {
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
                    onAccepted: Settings.subtitlesLanguage = subtitleModel.data(selectedIndex, "name")
                }
            }
        }

        MyScrollBar {
            flickableItem: flicker
        }
    }

    onAccepted: Transfers.addDownloadTransfer(Resources.VIMEO, resourceId, streamSelector.value.id, "",
                                              resourceTitle, Settings.defaultCategory,
                                              subtitleSwitch.checked ? Settings.subtitlesLanguage : "")

    onStatusChanged: {
        switch (status) {
        case DialogStatus.Open: {
            subtitleSwitch.checked = false;
            subtitleModel.clear();
            streamModel.list(resourceId);
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
