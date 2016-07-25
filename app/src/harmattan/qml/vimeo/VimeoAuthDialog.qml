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
import QtWebKit 1.0
import ".."

MySheet {
    id: root

    signal codeReady(string code)

    rejectButtonText: qsTr("Cancel")
    content: Item {
        anchors.fill: parent

        Flickable {
            id: flicker

            anchors.fill: parent
            contentWidth: webView.width
            contentHeight: webView.height
            boundsBehavior: Flickable.DragOverBounds
            clip: true
            visible: webView.url != ""

            WebView {
                id: webView

                width: 854
                height: 854
                preferredWidth: width
                preferredHeight: height
                settings.privateBrowsingEnabled: true
                opacity: status == WebView.Loading ? 0 : 1
                onUrlChanged: {
                    var s = url.toString();

                    if (s == "https://vimeo.com/") {
                        // Work-around as for some reason Vimeo sends you to the homepage on the first attempt.
                        url = Vimeo.authUrl();
                    }
                    else if (/code=/i.test(s)) {
                        root.codeReady(s.split("code=")[1].split("&")[0]);
                        root.accept();
                    }
                }
            }
        }

        ScrollDecorator {
            flickableItem: flicker
        }

        MyBusyIndicator {
            anchors.centerIn: parent
            size: "large"
            visible: (webView.status == WebView.Loading) && (root.status === DialogStatus.Open)
        }
    }

    onStatusChanged: {
        switch (status) {
        case DialogStatus.Open: {
            CookieJar.setAllCookies([]);
            webView.url = Vimeo.authUrl();
            return;
        }
        case DialogStatus.Closed:
            webView.url = "";
            return;
        default:
            return;
        }
    }
}
