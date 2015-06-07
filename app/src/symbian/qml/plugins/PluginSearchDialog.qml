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
import ".."


MySheet {
    id: root

    hideHeaderWhenInputContextIsVisible: false
    acceptButtonText: searchField.text ? qsTr("Done") : ""
    rejectButtonText: qsTr("Cancel")
    content: Item {
        anchors.fill: parent

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                topMargin: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge

            MyTextField {
                id: searchField

                x: platformStyle.paddingLarge
                width: parent.width - platformStyle.paddingLarge * 2
                clearButtonEnabled: true
                placeholderText: qsTr("Search")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                validator: RegExpValidator {
                    regExp: /^.+/
                }
                onTextChanged: searchModel.setFilterFixedString(text)
                onAccepted: {
                    closeSoftwareInputPanel();
                    root.accept();
                }
            }

            ValueSelector {
                id: typeSelector

                width: parent.width
                title: qsTr("Search for")
                model: PluginSearchTypeModel {
                    service: Settings.currentService
                }
                selectedIndex: Math.max(0, model.match("name",Settings.defaultSearchType(model.service)))
                onAccepted: Settings.setDefaultSearchType(model.service, model.data(selectedIndex, "name"))
            }
        }

        MyListView {
            id: view

            anchors {
                left: parent.left
                right: parent.right
                top: column.bottom
                topMargin: platformStyle.paddingLarge
                bottom: parent.bottom
            }
            clip: true
            model: SearchHistoryModel {
                id: searchModel
            }
            header: SeparatorLabel {
                text: qsTr("Search history")
                x: platformStyle.paddingLarge
                width: view.width - platformStyle.paddingLarge
            }
            delegate: LabelDelegate {
                onClicked: searchField.text = display
                onPressAndHold: {
                    view.currentIndex = -1;
                    view.currentIndex = index;
                    contextMenu.open();
                }
            }

            MyScrollBar {
                flickableItem: view
            }

            MyContextMenu {
                id: contextMenu

                focusItem: view

                MenuLayout {

                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: searchModel.removeSearch(view.currentIndex)
                    }

                    MenuItem {
                        text: qsTr("Clear")
                        onClicked: searchModel.clear()
                    }
                }
            }
        }

        states: State {
            name: "landscape"
            when: !appWindow.inPortrait

            AnchorChanges {
                target: column
                anchors.right: parent.horizontalCenter
            }

            AnchorChanges {
                target: view
                anchors {
                    left: column.right
                    top: parent.top
                }
            }
        }
    }

    onAccepted: {
        if (!mainPage.showResourceFromUrl(searchField.text)) {
            Settings.addSearch(searchField.text);
            mainPage.search(Settings.currentService, searchField.text, typeSelector.value.type, typeSelector.value.order);
        }
    }
    onStatusChanged: if (status == DialogStatus.Open) searchField.text = "";
}
