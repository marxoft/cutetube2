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

MyToolIcon {
    property Item pageToDestroy: null

    platformIconId: "toolbar-back"
    onClicked: {
        pageToDestroy = appWindow.pageStack.currentPage;
        appWindow.pageStack.pop();
    }

    Connections {
        target: pageToDestroy === null ? null : appWindow.pageStack
        onBusyChanged: {
            if ((!appWindow.pageStack.busy) && (pageToDestroy !== null)) {
                pageToDestroy.destroy();
                pageToDestroy = null;
            }
        }
    }
}
