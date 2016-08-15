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

MyPage {
    id: root
    
    title: qsTr("Log")
    tools: ToolBarLayout {

        BackToolButton {}
        
        MyToolButton {
            iconSource: "toolbar-delete"
            toolTipText: qsTr("Clear log")
            enabled: textArea.text != ""
            onClicked: {
                Logger.clear();
                textArea.text = "";
                infoBanner.showMessage(qsTr("Log cleared"));
            }
        }
    }

    TextArea {
        id: textArea

        anchors {
            fill: parent
            margins: platformStyle.paddingLarge
        }
        readOnly: true
    }

    onStatusChanged: if (status == PageStatus.Active) textArea.text = Logger.text;
}
