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
import com.nokia.symbian 1.1
import com.nokia.extras 1.1

AppWindow {
    id: appWindow
    
    showStatusBar: true
    showToolBar: true
    initialPage: MainPage {
        id: mainPage
    }

    InfoBanner {
        id: infoBanner

        function showMessage(message) {
            text = message;
            open();
        }
    }

    Connections {
        id: transferConnections
        
        target: null
        onTransferAdded: infoBanner.showMessage("'" + transfer.title + "' " + qsTr("added to transfers"))
    }

    Component.onCompleted: {
        Plugins.load();
        Transfers.restore();
        transferConnections.target = Transfers;

        var service = Settings.currentService;

        if (service) {
            mainPage.setService(service);
        }
        else {
            mainPage.setService(Resources.YOUTUBE);
        }
    }
}
