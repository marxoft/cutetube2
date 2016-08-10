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
import com.nokia.meego 1.0

Label {
    id: root

    property string link

    signal clicked

    color: mouseArea.pressed ? Qt.lighter(Settings.activeColor) : Settings.activeColor

    MouseArea {
        id: mouseArea

        anchors.fill: parent
        onClicked: {
            root.clicked();

            if (root.link) {
                Qt.openUrlExternally(root.link);
            }
        }
    }
}
