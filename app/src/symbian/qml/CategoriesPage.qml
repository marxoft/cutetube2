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
import com.nokia.symbian 1.1
import cuteTube 2.0

MyPage {
    id: root

    title: qsTr("Categories")
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-add"
            toolTipText: qsTr("Add category")
            onClicked: {
                loader.sourceComponent = editDialog;
                loader.item.name = "";
                loader.item.path = "";
                loader.item.open();
            }
        }
    }

    MyContextMenu {
        id: contextMenu

        focusItem: view

        MenuLayout {

            MenuItem {
                text: qsTr("Edit")
                onClicked: {
                    loader.sourceComponent = editDialog;
                    loader.item.name = categoryModel.data(view.currentIndex, "name");
                    loader.item.path = categoryModel.data(view.currentIndex, "path");
                    loader.item.open();
                }
            }

            MenuItem {
                text: qsTr("Remove")
                onClicked: categoryModel.removeCategory(view.currentIndex)
            }
        }
    }

    MyListView {
        id: view

        anchors.fill: parent
        model: CategoryModel {
            id: categoryModel
        }
        delegate: CategoryDelegate {
            onClicked: {
                loader.sourceComponent = editDialog;
                loader.item.name = name;
                loader.item.path = path;
                loader.item.open();
            }
            onPressAndHold: {
                view.currentIndex = index;
                contextMenu.open();
            }
        }
    }

    MyScrollBar {
        flickableItem: view
    }

    Label {
        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 40
        color: platformStyle.colorNormalMid
        text: qsTr("No categories")
        visible: categoryModel.count == 0
    }

    Loader {
        id: loader
    }

    Component {
        id: editDialog

        EditCategoryDialog {
            focusItem: view
        }
    }
}

