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

    title: qsTr("Categories")
    tools: ToolBarLayout {

        BackToolIcon {}

        MyToolIcon {
            platformIconId: "toolbar-add"
            onClicked: {
                loader.sourceComponent = editDialog;
                loader.item.name = "";
                loader.item.path = "";
                loader.item.open();
            }
        }
    }

    ContextMenu {
        id: contextMenu

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

    ListView {
        id: view

        anchors.fill: parent
        highlightFollowsCurrentItem: false
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

    ScrollDecorator {
        flickableItem: view
    }

    Label {
        anchors {
            fill: parent
            margins: UI.PADDING_DOUBLE
        }
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        wrapMode: Text.WordWrap
        font.pixelSize: 60
        color: UI.COLOR_INVERTED_SECONDARY_FOREGROUND
        text: qsTr("No categories")
        visible: categoryModel.count == 0
    }

    Loader {
        id: loader
    }

    Component {
        id: editDialog

        EditCategoryDialog {}
    }
}

