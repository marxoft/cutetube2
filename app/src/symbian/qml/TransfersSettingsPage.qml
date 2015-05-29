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

    title: qsTr("Transfers")
    tools: ToolBarLayout {

        BackToolButton {}
    }

    KeyNavFlickable {
        id: flicker

        anchors.fill: parent
        contentHeight: column.height + platformStyle.paddingLarge

        Column {
            id: column

            anchors {
                left: parent.left
                right: parent.right
                top: parent.top
            }

            ValueSelector {
                width: parent.width
                title: qsTr("Maximum concurrent transfers")
                model: ConcurrentTransfersModel {}
                focusItem: flicker
                value: Settings.maximumConcurrentTransfers
                onValueChanged: Settings.maximumConcurrentTransfers = value
            }

            Item {
                width: parent.width
                height: platformStyle.paddingLarge
            }

            MySwitch {
                width: parent.width
                text: qsTr("Start transfers automatically")
                checked: Settings.startTransfersAutomatically
                onCheckedChanged: Settings.startTransfersAutomatically = checked
            }
        }
    }

    MyScrollBar {
        flickableItem: flicker
    }
}
