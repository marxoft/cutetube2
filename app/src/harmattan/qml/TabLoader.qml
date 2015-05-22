/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

import QtQuick 1.1

Loader {
    id: root

    property string title
    property bool showProgressIndicator: item ? item.showProgressIndicator : false
    property Component tab: null
    property Item tools: item ? item.tools : null
    property bool loaded: sourceComponent == tab

    function loadTab() {
        sourceComponent = tab;
    }

    function unloadTab() {
        sourceComponent = undefined;
    }

    width: parent ? parent.width : undefined
    height: parent ? parent.width : undefined
}
