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

MySheet {
    id: root

    property alias name: nameField.text
    property string path

    rejectButtonText: qsTr("Cancel")
    acceptButtonText: (nameField.text == "") || (root.path == "") ? "" : qsTr("Done")
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
                    text: qsTr("Name")
                    visible: nameField.visible
                }

                MyTextField {
                    id: nameField

                    width: parent.width
                    visible: (!inputContext.visible) || (focus)
                    onAccepted: closeSoftwareInputPanel()
                }

                ValueListItem {
                    id: pathSelector

                    x: -platformStyle.paddingLarge
                    width: parent.width + platformStyle.paddingLarge * 2
                    title: qsTr("Download path")
                    subTitle: root.path == "" ? qsTr("None chosen") : root.path
                    visible: !inputContext.visible
                    onClicked: {
                        loader.sourceComponent = pathDialog;
                        loader.item.open();
                    }
                }
            }
        }

        MyScrollBar {
            flickableItem: flicker
        }
    }

    Loader {
        id: loader
    }

    Component {
        id: pathDialog

        FileBrowserDialog {
            focusItem: flicker
            startFolder: root.path == "" ? "/home/user/MyDocs/" : root.path
            onFileChosen: root.path = filePath
        }
    }

    onAccepted: Settings.addCategory(nameField.text, root.path)
}
