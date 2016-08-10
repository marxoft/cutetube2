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
import ".."

MyPage {
    id: root

    property variant plugin

    title: plugin ? plugin.displayName : ""
    tools: ToolBarLayout {
        BackToolButton {}
    }

    MyFlickable {
        id: flickable

        anchors.fill: parent
        height: column.height + platformStyle.paddingLarge

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
                margins: platformStyle.paddingLarge
            }
            spacing: platformStyle.paddingLarge

            Label {
                width: parent.width
                font.bold: true
                text: qsTr("Name")
            }

            Label {
                id: nameLabel

                width: parent.width - icon.width - platformStyle.paddingLarge
                height: icon.height
                verticalAlignment: Text.AlignVCenter
                elide: Text.ElideRight
                text: plugin ? plugin.displayName : ""
            }

            Label {
                width: parent.width
                font.bold: true
                text: qsTr("Plugin type")
            }

            Label {
                width: parent.width
                text: plugin ? plugin.pluginType : ""
            }

            Label {
                width: parent.width
                font.bold: true
                text: qsTr("Version")
            }

            Label {
                width: parent.width
                text: plugin ? plugin.version : ""
            }
        }
    }
}
