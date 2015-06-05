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

import QtQuick 2.0
import QtQuick.Controls 1.1

ToolButton {
    id: viewButton

    text: qsTr("View")
    iconName: "gtk-index"
    menu: Menu {
    
        ExclusiveGroup {
            id: viewGroup
        }
        
        MenuItem {
            text: qsTr("List")
            checkable: true
            checked: Settings.defaultViewMode == "list"
            exclusiveGroup: viewGroup
            onTriggered: Settings.defaultViewMode = "list"
        }
        
        MenuItem {
            text: qsTr("Grid")
            checkable: true
            checked: Settings.defaultViewMode == "grid"
            exclusiveGroup: viewGroup
            onTriggered: Settings.defaultViewMode = "grid"
        }
    }
}
