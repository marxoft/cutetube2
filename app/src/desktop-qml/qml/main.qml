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
import QtQml 2.2
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import cuteTube 2.0

ApplicationWindow {
    id: window
    
    function search(service, query, type, order) {
        raise();
        setService(service);
        return pageStack.currentItem.search(query, type, order);
    }
    
    function setService(service) {
        if (!service) {
            return;
        }
        
        Settings.currentService = service;        
        pageStack.clear();
        
        if (service == Resources.YOUTUBE) {
            pageStack.push({item: Qt.resolvedUrl("youtube/YouTubePage.qml"), immediate: true});
        }
        else if (service == Resources.DAILYMOTION) {
            pageStack.push({item: Qt.resolvedUrl("dailymotion/DailymotionPage.qml"), immediate: true});
        }
        else if (service == Resources.VIMEO) {
            pageStack.push({item: Qt.resolvedUrl("vimeo/VimeoPage.qml"), immediate: true});
        }
        else {
            pageStack.push({item: Qt.resolvedUrl("plugins/PluginPage.qml"), properties: {service: service}, immediate: true});
        }
    }    
    
    function showResource(resource) {
        if (!resource.service) {
            return false;
        }
        
        raise();
        setService(resource.service);
        return pageStack.currentItem.showResource(resource);
    }
    
    function showResourceFromUrl(url) {
        return showResource(Resources.getResourceFromUrl(url));
    }
    
    minimumWidth: 1000
    minimumHeight: 600
    visible: true
    title: "cuteTube2"
    menuBar: MenuBar {
        
        Menu {
            id: fileMenu
            
            title: qsTr("File")
            
            MenuItem {
                text: qsTr("Open URL")
                iconName: "applications-internet"
                shortcut: "Ctrl+U"
                onTriggered: {
                    loader.sourceComponent = urlDialog;
                    loader.item.open();
                }
            }
            
            MenuItem {
                text: qsTr("Quit")
                iconName: "application-exit"
                shortcut: "Ctrl+Q"
                onTriggered: Qt.quit()
            }
        }
        
        Menu {
            id: viewMenu
            
            title: qsTr("View")
                        
            MenuItem {
                text: qsTr("View transfers")
                iconName: "folder-download"
                shortcut: "Ctrl+T"
                checkable: true
                onTriggered: {
                    if (checked) {
                        pageStack.push({item: Qt.resolvedUrl("TransfersPage.qml"), immediate: true});
                    }
                    else {
                        pageStack.pop({immediate: true});
                    }
                }
            }
        }
        
        Menu {
            id: editMenu
            
            title: qsTr("Edit")
            
            MenuItem {
                text: qsTr("Preferences")
                iconName: "preferences-desktop"
                shortcut: "Ctrl+P"
                onTriggered: {
                    loader.sourceComponent = settingsDialog;
                    loader.item.open();
                }
            }
        }
        
        Menu {
            id: helpMenu
            
            title: qsTr("Help")
            
            MenuItem {
                text: qsTr("About")
                iconName: "help-about"
                onTriggered: {
                    loader.sourceComponent = aboutDialog;
                    loader.item.open();
                }
            }
        }
    }
    
    statusBar: StatusBar {
        id: statusBar
        
        function showMessage(message) {
            statusLabel.text = message;
            statusTimer.restart();
        }
        
        Timer {
            id: statusTimer
            
            interval: 3000
            onTriggered: statusLabel.text = ""
        }
        
        RowLayout {
            
            Label {
                id: statusLabel
                
                font.italic: true
            }
        }
    }
    
    StateGroup {
        id: windowStates
    
        states: State {
            name: "playerMaximised"
    
            PropertyChanges {
                target: fileMenu
                visible: false
            }
            
            PropertyChanges {
                target: viewMenu
                visible: false
            }
    
            PropertyChanges {
                target: editMenu
                visible: false
            }
    
            PropertyChanges {
                target: helpMenu
                visible: false
            }
        
            PropertyChanges {
                target: statusBar
                visible: false
            }
        }
    }
        
    SystemPalette {
        id: palette
    }
    
    StackView {
        id: pageStack
    }
    
    MessageBox {
        id: messageBox
    }
    
    Loader {
        id: loader
    }

    Component {
        id: urlDialog
        
        UrlDialog {
            onAccepted: showResourceFromUrl(url)
        }
    }
    
    Component {
        id: settingsDialog
        
        SettingsDialog {}
    }
    
    Component {
        id: aboutDialog
        
        AboutDialog {}
    }
    
    Connections {
        target: Clipboard
        onTextChanged: showResourceFromUrl(text)
    }
    
    Connections {
        target: DBus
        onResourceRequested: showResource(resource)
    }
    
    Connections {
        target: Transfers
        onTransferAdded: statusBar.showMessage("'" + transfer.title + "' " + qsTr("added to transfers"))
    }
    
    Component.onCompleted: {
        Transfers.restoreTransfers();
        
        if (DBus.requestedResource.service) {
            showResource(DBus.requestedResource);
        }
        else {
            var service = Settings.currentService;
        
            if (service) {
                setService(service);
            }
            else {
                setService(Resources.YOUTUBE);
            }
        }
    }
}
    