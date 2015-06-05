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
import QtQuick.Controls 1.0

ScrollView {
    id: root
    
    property string mode: "list"
    property alias model: view.model
    property alias delegate: view.delegate
    property alias currentItem: view.currentItem
    property alias currentIndex: view.currentIndex
    property alias cellWidth: view.cellWidth
    property alias cellHeight: view.cellHeight
    
    focus: true

    GridView {
        id: view        
    
        cellWidth: currentItem ? currentItem.width + (mode == "grid" ? 10 : 0) : width
        cellHeight: currentItem ? currentItem.height + (mode == "grid" ? 10 : 0) : 30
        focus: true
        cacheBuffer: 500
        highlightMoveDuration: 0
        highlight: BorderImage {
            border {
                left: 4
                right:4
            }
            height: 16
            source: "image://__tablerow/selected_" + ((window.active) && (view.focus) ? "active" : "")
        }
        
        Keys.onPressed: {
            switch (event.key) {
            case Qt.Key_Up:
                moveCurrentIndexUp();
                break;
            case Qt.Key_Down:
                moveCurrentIndexDown();
                break;
            case Qt.Key_Left:
                moveCurrentIndexLeft();
                break;
            case Qt.Key_Right:
                moveCurrentIndexRight();
                break;
            case Qt.Key_Enter:
            case Qt.Key_Return:
                if (currentItem) currentItem.clicked();
                break;
            default:
                break;
            }
        }
    }        
}
