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
import QYouTube 1.0 as QYouTube
import ".."
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MySheet {
    id: root

    function list(resourceId, title) {
        internal.resourceId = resourceId;
        internal.title = title;
        streamModel.list(resourceId);
    }

    showProgressIndicator: (streamModel.status == QYouTube.StreamsRequest.Loading)
                           || (subtitleModel.status == QYouTube.SubtitlesRequest.Loading)
    acceptButtonText: streamModel.status == QYouTube.StreamsRequest.Ready ? qsTr("Done") : ""
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
                    model: YouTubeStreamModel {
                        id: streamModel

                        onStatusChanged: {
                            switch (status) {
                            case QYouTube.ResourcesRequest.Loading:
                                streamSelector.showProgressIndicator = true;
                                return;
                            case QYouTube.ResourcesRequest.Ready:
                                if (count > 0) {
                                    streamSelector.selectedIndex = Math.max(0, match("name",
                                                                            Settings.defaultDownloadFormat(Resources.YOUTUBE)));
                                }
                                else {
                                    infoBanner.showMessage(qsTr("No streams found"));
                                }

                                break;
                            case QYouTube.ResourcesRequest.Failed: {
                                infoBanner.showMessage(errorString);
                                break;
                            }
                            default:
                                break;
                            }

                            streamSelector.showProgressIndicator = false;
                        }
                    }
                    onValueChanged: Settings.setDefaultDownloadFormat(Resources.YOUTUBE, subTitle)
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

                MySwitch {
                    id: subtitleSwitch

                    text: qsTr("Download subtitles")
                    onCheckedChanged: {
                        if (checked) {
                            if (subtitleModel.status != QYouTube.SubtitlesRequest.Loading) {
                                subtitleModel.list(internal.resourceId);
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
                    model: YouTubeSubtitleModel {
                        id: subtitleModel

                        onStatusChanged: {
                            switch (status) {
                            case QYouTube.SubtitlesRequest.Loading:
                                subtitleSelector.showProgressIndicator = true;
                                return;
                            case QYouTube.SubtitlesRequest.Ready:
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
                            case QYouTube.SubtitlesRequest.Failed: {
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
            }
        }

        ScrollDecorator {
            flickableItem: flicker
        }
    }

    QtObject {
        id: internal

        property string resourceId
        property string title
    }

    onAccepted: Transfers.addDownloadTransfer(Resources.YOUTUBE, internal.resourceId, streamSelector.value.id,
                                              internal.title, Settings.defaultCategory,
                                              subtitleSwitch.checked ? Settings.subtitlesLanguage : "")

    onStatusChanged: {
        if (status == DialogStatus.Closing) {
            if (streamModel.status == QYouTube.StreamsRequest.Loading) {
                streamModel.cancel();
            }

            if (subtitleModel.status == QYouTube.SubtitlesRequest.Loading) {
                subtitleModel.cancel();
            }
        }
    }
}
