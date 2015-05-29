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
import ".."

MyPage {
    id: root

    title: qsTr("Plugins")
    tools: ToolBarLayout {

        BackToolButton {}
    }

    MyListView {
        id: view

        anchors.fill: parent
        model: PluginSettingsModel {
            id: pluginModel
        }
        delegate: DrillDownDelegate {
            text: name
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("PluginSettingsPage.qml")).loadSettings(name, value)
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
        text: qsTr("No plugins")
        visible: pluginModel.count == 0
    }
}
