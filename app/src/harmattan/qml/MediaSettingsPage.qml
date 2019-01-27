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
import com.nokia.meego 1.0
import cuteTube 2.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

MyPage {
    id: root

    title: qsTr("Media/content")
    tools: ToolBarLayout {

        BackToolIcon {}
    }

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

            ValueListItem {
                width: parent.width
                title: qsTr("Default download path")
                subTitle: Settings.downloadPath
                onClicked: {
                    loader.sourceComponent = pathDialog;
                    loader.item.open();
                }
            }

            DrillDownDelegate {
                width: parent.width
                text: qsTr("Categories")
                onClicked: appWindow.pageStack.push(Qt.resolvedUrl("CategoriesPage.qml"))
            }
            
            ValueSelector {
                width: parent.width
                title: qsTr("Language filter")
                model: LocaleModel {}
                value: Settings.locale
                onValueChanged: Settings.locale = value
            }

            ValueSelector {
                width: parent.width
                title: qsTr("Video player")
                model: VideoPlayerModel {}
                value: Settings.videoPlayer
                onValueChanged: Settings.videoPlayer = value
            }

            Item {
                width: parent.width
                height: UI.PADDING_DOUBLE
            }

            Label {
                x: UI.PADDING_DOUBLE
                font.bold: true
                text: qsTr("Custom command (%u for URI)")
            }

            Item {
                width: parent.width
                height: UI.PADDING_DOUBLE
            }

            MyTextField {
                x: UI.PADDING_DOUBLE
                width: parent.width - UI.PADDING_DOUBLE * 2
                text: Settings.videoPlayerCommand
                onTextChanged: Settings.videoPlayerCommand = text
                onAccepted: platformCloseSoftwareInputPanel()
            }
            
            Item {
                width: parent.width
                height: UI.PADDING_DOUBLE
            }

            MySwitch {
                width: parent.width
                text: qsTr("Enable safe search")
                checked: Settings.safeSearchEnabled
                onCheckedChanged: Settings.safeSearchEnabled = checked
            }
            
            Item {
                width: parent.width
                height: UI.PADDING_DOUBLE
            }

            MySwitch {
                width: parent.width
                text: qsTr("Monitor clipboard for URLs")
                checked: Settings.clipboardMonitorEnabled
                onCheckedChanged: Settings.clipboardMonitorEnabled = checked
            }
        }
    }

    ScrollDecorator {
        flickableItem: flicker
    }

    Loader {
        id: loader
    }

    Component {
        id: pathDialog

        FileBrowserDialog {
            onFileChosen: Settings.downloadPath = filePath
        }
    }
}
