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

MyListItem {
    id: root

    height: flow.height + platformStyle.paddingLarge * 2

    Flow {
        id: flow

        anchors {
            left: parent.left
            leftMargin: platformStyle.paddingLarge
            right: parent.right
            rightMargin: platformStyle.paddingLarge
            verticalCenter: parent.verticalCenter
        }

        MyListItemText {
            id: titleLabel

            width: parent.width - icon.width
            mode: root.mode
            role: "Title"
            text: title
        }

        Image {
            id: icon

            width: height
            height: titleLabel.height
            source: transferType === Transfer.Upload ? "images/upload.png" : "images/download.png"
            smooth: true
        }

        MyListItemText {
            id: statusLabel

            width: parent.width - sizeLabel.width
            mode: root.mode
            role: "SubTitle"
            text: statusString
        }

        MyListItemText {
            id: sizeLabel

            horizontalAlignment: Text.AlignRight
            mode: root.mode
            role: "SubTitle"
            text: size <= 0 ? "" : Utils.formatBytes(size)
        }

        ProgressBar {
            id: progressBar

            width: parent.width
            minimumValue: 0
            maximumValue: 100
            value: progress
        }
    }
}
