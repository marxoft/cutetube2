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
import ".."

Item {
    id: root

    property alias title: titleLabel.text
    property string key

    function setKey(key, defaultValue) {
        root.key = key;
        var value = Settings.value(key);

        if (value === undefined) {
            value = defaultValue;
        }

        textField.text = value;
    }

    height: titleLabel.height + textField.height + platformStyle.paddingLarge * 2
    width: !parent ? implicitWidth : parent.width
    visible: (!inputContext.visble) || (focus)

    Label {
        id: titleLabel

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            top: parent.top
        }

        font.bold: true
        elide: Text.ElideRight
    }

    MyTextField {
        id: textField

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            top: titleLabel.bottom
            topMargin: platformStyle.paddingLarge
        }

        inputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase
        onTextChanged: Settings.setValue(root.key, textField.text)
        onAccepted: closeSoftwareInputPanel()
    }
}
