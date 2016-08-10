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

Item {
    id: root

    default property alias tabs: tabModel.children
    property alias currentTab: view.currentItem
    property alias currentIndex: view.currentIndex
    property alias delayInterval: loadingTimer.interval

    function unloadTabs() {
        for (var i = 0; i < tabs.length; i++) {
            if (tabs[i].hasOwnProperty("loaded")) {
                tabs[i].unloadTab();
            }
        }
    }

    clip: true

    ListView {
        id: view

        anchors.fill: parent
        model: VisualItemModel {
            id: tabModel
        }
        orientation: ListView.Horizontal
        snapMode: ListView.SnapOneItem
        boundsBehavior: ListView.StopAtBounds
        highlightRangeMode: ListView.StrictlyEnforceRange
        preferredHighlightBegin: 0
        preferredHighlightEnd: width
        onCurrentIndexChanged: {
            if ((currentItem) && (currentItem.hasOwnProperty("loaded")) && (!currentItem.loaded)) {
                loadingTimer.restart();
            }
            else {
                loadingTimer.stop();
            }
        }
    }

    Timer {
        id: loadingTimer

        interval: 1000
        onTriggered: if (view.currentItem) view.currentItem.loadTab();
    }
}
