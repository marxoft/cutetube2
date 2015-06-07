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
        var filters = {};
        filters["query"] = query;
        filters["sort"] = order;
        filters["per_page"] = MAX_RESULTS;

        view.currentIndex = 1;
        
        if (pageStack.depth > 0) {
            pageStack.clear();
        }
            
        if (type == Resources.USER) {
            pageStack.push({item: Qt.resolvedUrl("VimeoUsersPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                          .model.list("/users", filters);
        }
        else {
            pageStack.push({item: Qt.resolvedUrl("VimeoVideosPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                          .model.list("/videos", filters);
        }
        
        return true;
    }

    function showResource(resource) {
        var url;

        if (resource.type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("VimeoPlaylistPage.qml");
        }
        else if (resource.type == Resources.USER) {
            url = Qt.resolvedUrl("VimeoUserPage.qml");
        }
        else {
            url = Qt.resolvedUrl("VimeoVideoPage.qml");
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
            currentIndex: serviceModel.match("value", Resources.VIMEO)
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
            model: VimeoSearchTypeModel {
                id: searchTypeModel
            }
            textRole: "name"
            currentIndex: searchTypeModel.match("name", Settings.defaultSearchType(Resources.VIMEO))
            onActivated: Settings.setDefaultSearchType(Resources.VIMEO, searchTypeModel.data(index, "name"))
        }
        
        TextField {
            id: searchField
            
            Layout.minimumWidth: 300
            placeholderText: qsTr("Search")
            validator: RegExpValidator {
                regExp: /^.+/
            }
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
        model: VimeoNavModel {
            id: navModel
        }
        delegate: LabelDelegate {
            text: display
        }
        currentIndex: 1
        onCurrentIndexChanged: {
            pageStack.clear();
            
            switch (currentIndex) {
            case 0:
                pageStack.push({item: Qt.resolvedUrl("VimeoAccountsPage.qml"), immediate: true});
                break;
            case 1:
                break;
            case 2:
                pageStack.push({item: Qt.resolvedUrl("VimeoCategoriesPage.qml"), immediate: true})
                              .model.list("/categories", {per_page: 50});
                break;
            case 3:
                pageStack.push({item: Qt.resolvedUrl("VimeoVideosPage.qml"), properties: {title: qsTr("My videos")},
                                immediate: true}).model.list("/me/videos", {per_page: MAX_RESULTS});
                break;
            case 4:
                pageStack.push({item: Qt.resolvedUrl("VimeoVideosPage.qml"), properties: {title: qsTr("Latest videos")},
                                immediate: true}).model.list("/me/feed", {per_page: MAX_RESULTS});
                break;
            case 5:
                pageStack.push({item: Qt.resolvedUrl("VimeoVideosPage.qml"), properties: {title: qsTr("Likes")},
                                immediate: true}).model.list("/me/likes", {per_page: MAX_RESULTS});
                break;
            case 6:
                pageStack.push({item: Qt.resolvedUrl("VimeoVideosPage.qml"), properties: {title: qsTr("Watch later")},
                                immediate: true}).model.list("/me/watchlater", {per_page: MAX_RESULTS});
                break;
            case 7:
                pageStack.push({item: Qt.resolvedUrl("VimeoPlaylistsPage.qml"), immediate: true})
                .model.list("/me/albums", {per_page: MAX_RESULTS});
                break;
            case 8:
                pageStack.push({item: Qt.resolvedUrl("VimeoUsersPage.qml"), properties: {title: qsTr("Subscriptions")},
                                immediate: true}).model.list("/me/following", {per_page: MAX_RESULTS});
                break;
            default:
                break;
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
    
    Connections {
        target: Vimeo
        onCommentAdded: statusBar.showMessage(qsTr("Your comment has been added"))
        onUserSubscribed: statusBar.showMessage(qsTr("You have subscribed to") + " " + user.username)
        onUserUnsubscribed: statusBar.showMessage(qsTr("You have unsubscribed to") + " " + user.username)
        onVideoFavourited: statusBar.showMessage("'" + video.title + "' " + qsTr("added to likes"))
        onVideoUnfavourited: statusBar.showMessage("'" + video.title + "' " + qsTr("removed from likes"))
        onVideoWatchLater: statusBar.showMessage("'" + video.title + "' " + qsTr("added to your watch later album"))
        onVideoAddedToPlaylist: statusBar.showMessage("'" + video.title + "' " + qsTr("added to album") + " '" + playlist.title + "'")
    }
}
