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

import QtQuick 1.1
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

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

        Flickable {
            id: flicker

            anchors.fill: parent
            contentHeight: column.height + UI.PADDING_DOUBLE

            Column {
                id: column

                anchors {
                    top: parent.top
                    left: parent.left
                    right: parent.right
                    margins: UI.PADDING_DOUBLE
                }
                spacing: UI.PADDING_DOUBLE

                Label {
                    font.bold: true
                    text: qsTr("Name")
                }

                MyTextField {
                    id: nameField

                    width: parent.width
                }

                ValueListItem {
                    id: pathSelector

                    x: -UI.PADDING_DOUBLE
                    width: parent.width + UI.PADDING_DOUBLE * 2
                    title: qsTr("Download path")
                    subTitle: root.path == "" ? qsTr("None chosen") : root.path
                    onClicked: {
                        loader.sourceComponent = pathDialog;
                        loader.item.open();
                    }
                }
            }
        }

        ScrollDecorator {
            flickableItem: flicker
        }
    }

    Loader {
        id: loader
    }

    Component {
        id: pathDialog

        FileBrowserDialog {
            startFolder: root.path == "" ? "/home/user/MyDocs/" : root.path
            onFileChosen: root.path = filePath
        }
    }

    onAccepted: Settings.addCategory(nameField.text, root.path)
}
