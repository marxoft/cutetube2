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
import com.nokia.meego 1.0
import "file:///usr/lib/qt4/imports/com/nokia/meego/UIConstants.js" as UI

Item {
    id: root

    property alias title: titleLabel.text
    property alias minimumValue: slider.minimumValue
    property alias maximumValue: slider.maximumValue
    property alias stepSize: slider.stepSize
    property string key

    function setKey(key, defaultValue) {
        root.key = key;
        var value = Settings.value(key);

        if (value === undefined) {
            slider.value = parseFloat(defaultValue);
        }
        else {
            slider.value = parseFloat(value);
        }
    }

    height: 84
    width: !parent ? implicitWidth : parent.width

    Label {
        id: titleLabel

        anchors {
            left: parent.left
            leftMargin: UI.PADDING_DOUBLE
            right: parent.right
            rightMargin: UI.PADDING_DOUBLE
            top: parent.top
        }

        font.bold: true
        elide: Text.ElideRight
    }

    Slider {
        id: slider

        anchors {
            left: parent.left
            leftMargin: 30
            right: parent.right
            rightMargin: 30
            top: parent.verticalCenter
        }
        orientation: Qt.Horizontal
        valueIndicatorVisible: true
        onValueChanged: Settings.setValue(root.key, slider.value)
    }
}
