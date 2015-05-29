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

    title: qsTr("Appearance")
    tools: ToolBarLayout {
        id: toolBar

        BackToolButton {}
    }

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
            }

            ValueSelector {
                width: parent.width
                title: qsTr("Screen orientation")
                model: ScreenOrientationModel {}
                focusItem: flicker
                value: Settings.screenOrientation
                onValueChanged: Settings.screenOrientation = value
            }
        }
    }

    MyScrollBar {
        flickableItem: flicker
    }
}
