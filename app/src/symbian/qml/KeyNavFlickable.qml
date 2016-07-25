/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1
import com.nokia.symbian 1.1

Flickable {
    id: root

    property ScrollBar scrollIndicator: null
    property int currentIndex
    property bool loaderFix: false

    onVisibleChanged: {
        if (visible) {
            if (internal.focusItem) {
                internal.focusItem.forceActiveFocus();
            }
            else {
                forceActiveFocus();
            }
        }
    }
    onCurrentIndexChanged: internal.focusItem = internal.focusItems[currentIndex];
    Component.onCompleted: internal.getKeyboardFocus()

    QtObject {
        id: internal

        property variant focusItems: []
        property Item focusItem: null

        function itemGetsFocus(item) {
            return (item.hasOwnProperty("checked")) || (item.hasOwnProperty("cursorPosition"))
                    || (item.hasOwnProperty("subItemIndicator")) || (item.hasOwnProperty("item"))
                    || (item.hasOwnProperty("model"));
        }

        function itemIsLayout(item) {
            return item.hasOwnProperty("spacing");
        }

        function getFocusItems(item) {
            if (!item) {
                item = contentItem;
            }

            for (var i = 0; i < item.children.length; i++) {
                if (itemGetsFocus(item.children[i])) {
                    var fi = focusItems;
                    fi.push(item.children[i]);
                    focusItems = fi;
                }
                else if (itemIsLayout(item.children[i])) {
                    getFocusItems(item.children[i]);
                }
            }
        }

        function getKeyboardFocus() {
            getFocusItems(contentItem);

            if (focusItems.length > currentIndex) {
                if (loaderFix) {
                    var fi = focusItems;
                    fi.unshift(fi.pop());
                    focusItems = fi;
                }

                focusItem = focusItems[currentIndex];
            }
            else {
                forceActiveFocus();
            }
        }

        onFocusItemChanged: {
            if (focusItem) {
                focusItem.forceActiveFocus();

                if (currentIndex == 0) {
                    contentY = 0;
                }
                else if (contentY + height < mapFromItem(focusItem, focusItem.x, focusItem.y).y) {
                    contentY = mapFromItem(focusItem, focusItem.x, focusItem.y).y - height;
                }
                else if (contentY > focusItem.y) {
                    contentY = focusItem.y;
                }
            }
        }
    }

    function next() {
        for (var i = currentIndex + 1; i < internal.focusItems.length; i++) {
            if ((internal.focusItems[i].visible) && (internal.focusItems[i].enabled)) {
                currentIndex = i;
                return;
            }
        }
    }

    function previous() {
        for (var i = currentIndex - 1; i >= 0; i--) {
            if ((internal.focusItems[i].visible) && (internal.focusItems[i].enabled)) {
                currentIndex = i;
                return;
            }
        }
    }

    function moveUp() {
        if (contentY > 100) {
            contentY -= 100;
        }
        else {
            contentY = 0;
        }
    }

    function moveDown() {
        if (contentY < contentHeight - height - 100) {
            contentY += 100;
        }
        else {
            contentY = contentHeight - height;
        }
    }

    Keys.onUpPressed: {
        if (symbian.listInteractionMode !== Symbian.KeyNavigation) {
            symbian.listInteractionMode = Symbian.KeyNavigation;
        }
        if (scrollIndicator) {
            scrollIndicator.flash();
        }
        if (internal.focusItems.length > 0) {
            previous();
        }
        else {
            moveUp();
        }

        event.accepted = true;
    }
    Keys.onDownPressed: {
        if (symbian.listInteractionMode !== Symbian.KeyNavigation) {
            symbian.listInteractionMode = Symbian.KeyNavigation;
        }
        if (scrollIndicator) {
            scrollIndicator.flash();
        }
        if (internal.focusItems.length > 0) {
            next();
        }
        else {
            moveDown();
        }

        event.accepted = true;
    }

    Connections {
        target: inputContext
        onVisibleChanged: if ((!inputContext.visible) && (visible)) forceActiveFocus();
    }

    Behavior on contentY { NumberAnimation { duration: 250 } }

    MouseArea {
        z: Number.MAX_VALUE
        anchors.fill: parent
        onPressed: {
            if (symbian.listInteractionMode !== Symbian.TouchInteraction) {
                symbian.listInteractionMode = Symbian.TouchInteraction;
            }

            mouse.accepted = inputContext.visible;
        }
        onReleased: root.forceActiveFocus();
    }
}

