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
import QtQuick.Controls 1.1
import QtQuick.Layouts 1.1
import cuteTube 2.0
import ".."

Page {
    id: root
    
    function search(query, type, order) {
        view.currentIndex = 1;
        
        if (pageStack.depth > 0) {
            pageStack.clear();
        }
        
        if (type == Resources.PLAYLIST) {
            pageStack.push({item: Qt.resolvedUrl("PluginPlaylistsPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                          .model.search(query, order);
        }
        else if (type == Resources.USER) {
            pageStack.push({item: Qt.resolvedUrl("PluginUsersPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                          .model.search(query, order);
        }
        else {
            pageStack.push({item: Qt.resolvedUrl("PluginVideosPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                          .model.search(query, order);
        }
    }

    function showResource(resource) {
        var url;

        if (resource.type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("PluginPlaylistPage.qml");
        }
        else if (resource.type == Resources.USER) {
            url = Qt.resolvedUrl("PluginUserPage.qml");
        }
        else {
            url = Qt.resolvedUrl("PluginVideoPage.qml");
        }

        if (pageStack.depth > 0) {
            pageStack.clear();
        }
        
        pageStack.push({item: url, immediate: true}).load(resource.id);
        return true;
    }
    
    tools: ToolBarLayout {

        ComboBox {
            id: serviceSelector
            
            model: ServiceModel {
                id: serviceModel
            }
            textRole: "name"
            currentIndex: serviceModel.match("value", Settings.currentService)
            onActivated: window.setService(serviceModel.data(index, "value"))
        }
        
        Item {
            Layout.fillWidth: true
        }
        
        Label {
            text: qsTr("Search for")
        }
        
        ComboBox {
            id: searchTypeSelector
            
            Layout.minimumWidth: 200
            model: PluginSearchTypeModel {
                id: searchTypeModel
                
                service: Settings.currentService
            }
            textRole: "name"
            currentIndex: searchTypeModel.match("name", Settings.defaultSearchType(searchTypeModel.service))
            enabled: searchTypeModel.count > 0
            onActivated: Settings.setDefaultSearchType(searchTypeModel.service, searchTypeModel.data(index, "name"))
        }
        
        Label {
            text: qsTr("Order by")
        }
        
        TextField {
            id: searchField
            
            Layout.minimumWidth: 300
            placeholderText: qsTr("Search")
            validator: RegExpValidator {
                regExp: /^.+/
            }
            enabled: searchTypeModel.count > 0
            onAccepted: {
                root.search(text, searchTypeModel.data(searchTypeSelector.currentIndex, "value").type,
                            searchTypeModel.data(searchTypeSelector.currentIndex, "value").order);
                text = "";
            }
        }        
    }
    
    ItemView {
        id: view
        
        width: serviceSelector.width + 10
        anchors {
            left: parent.left
            top: parent.top
            bottom: parent.bottom
        }
        model: PluginNavModel {
            id: navModel
            
            service: Settings.currentService
        }
        delegate: LabelDelegate {
            text: name
        }
        onCurrentIndexChanged: {
            pageStack.clear();
            
            switch (currentIndex) {
            case 0:
                if (searchTypeModel.count > 0) {
                    return;
                }
                
                break;
            default:
                break;
            }
            
            var value = navModel.data(currentIndex, "value");
            
            if (value.type == Resources.CATEGORY) {
                pageStack.push({item: Qt.resolvedUrl("PluginCategoriesPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
            else if (value.type == Resources.PLAYLIST) {
                pageStack.push({item: Qt.resolvedUrl("PluginPlaylistsPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
            else if (value.type == Resources.USER) {
                pageStack.push({item: Qt.resolvedUrl("PluginUsersPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
            else {
                pageStack.push({item: Qt.resolvedUrl("PluginVideosPage.qml"), properties: {title: value.name},
                               immediate: true}).model.list(value.id);
            }
        }
    }
    
    StackView {
        id: pageStack
        
        anchors {
            left: view.right
            right: parent.right
            top: parent.top
            bottom: parent.bottom
        }
    }
}
