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

ValueListItem {
    id: root

    property string key
    property variant value
    property int selectedIndex: -1
    property Item focusItem: null

    function setList(key, defaultValue, list) {
        root.key = key;

        var value = Settings.value(key);

        if (value === undefined) {
            value = defaultValue;
            Settings.setValue(key, value);
        }

        for (var i = 0; i < list.length; i++) {
            selectionModel.append(list[i].name, list[i].value);

            if (list[i].value === value) {
                root.value = value;
                root.subTitle = list[i].name;
            }
        }
    }

    SelectionModel {
        id: selectionModel
    }

    Loader {
        id: loader
    }

    Component {
        id: dialog

        ValueDialog {
            titleText: root.title
            model: selectionModel
            value: root.value
            focusItem: root.focusItem
            selectedIndex: root.selectedIndex
            onSelectedIndexChanged: root.selectedIndex = selectedIndex
            onValueChanged: {
                root.value = value;
                Settings.setValue(root.key, value);
            }
        }
    }

    visible: !inputContext.visible
    onSelectedIndexChanged: if (selectionModel) value = selectionModel.data(selectedIndex, "value");
    onValueChanged: if (selectionModel.count > 0) subTitle = selectionModel.data(selectionModel.match("value", value), "name");
    onClicked: {
        loader.sourceComponent = dialog;
        loader.item.open();
    }
}
