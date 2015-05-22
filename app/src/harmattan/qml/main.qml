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
import com.nokia.meego 1.0
import com.nokia.extras 1.1

AppWindow {
    id: appWindow
    
    showStatusBar: true
    showToolBar: true
    initialPage: MainPage {
        id: mainPage
    }
    platformStyle: PageStackWindowStyle {
        id: appStyle

        background: "image://theme/meegotouch-applicationpage-background-inverted"
        backgroundFillMode: Image.Stretch
        cornersVisible: true
    }

    TitleHeader {
        id: titleHeader
    }

    InfoBanner {
        id: infoBanner

        function showMessage(message) {
            text = message;
            show();
        }

        topMargin: 40
    }

    Connections {
        target: Clipboard
        onTextChanged: mainPage.showResourceFromUrl(text)
    }

    Connections {
        target: DBus
        onResourceRequested: mainPage.showResource(resource)
    }

    Connections {
        target: Transfers
        onTransferAdded: infoBanner.showMessage("'" + transfer.title + "' " + qsTr("added to transfers"))
    }

    Component.onCompleted: {
        theme.inverted = true;
        Transfers.restoreTransfers();

        if (DBus.requestedResource.service) {
            mainPage.showResource(DBus.requestedResource);
        }
        else {
            var service = Settings.currentService;

            if (service) {
                mainPage.setService(service);
            }
            else {
                mainPage.setService(Resources.YOUTUBE);
            }
        }
    }
}
