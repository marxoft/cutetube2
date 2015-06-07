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
        var url;
        var filters = {};
        filters["search"] = query;
        filters["sort"] = order;
        filters["limit"] = MAX_RESULTS;
        filters["family_filter"] = Settings.safeSearchEnabled;
        filters["localization"] = Settings.locale;
        
        view.currentIndex = 1;
        
        if (pageStack.depth > 0) {
            pageStack.clear();
        }

        if (type == Resources.PLAYLIST) {
            pageStack.push({item: Qt.resolvedUrl("DailymotionPlaylistsPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                            .model.list("/playlists", filters);
        }
        else if (type == Resources.USER) {
            pageStack.push({item: Qt.resolvedUrl("DailymotionUsersPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                            .model.list("/users", filters);
        }
        else {
            pageStack.push({item: Qt.resolvedUrl("DailymotionVideosPage.qml"),
                            properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                            .model.list("/videos", filters);
        }        
        
        return true;
    }

    function showResource(resource) {
        var url;

        if (resource.type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("DailymotionPlaylistPage.qml");
        }
        else if (resource.type == Resources.USER) {
            url = Qt.resolvedUrl("DailymotionUserPage.qml");
        }
        else {
            url = Qt.resolvedUrl("DailymotionVideoPage.qml");
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
            currentIndex: serviceModel.match("value", Resources.DAILYMOTION)
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
            model: DailymotionSearchTypeModel {
                id: searchTypeModel
            }
            textRole: "name"
            currentIndex: searchTypeModel.match("name", Settings.defaultSearchType(Resources.DAILYMOTION))
            onActivated: Settings.setDefaultSearchType(Resources.DAILYMOTION, searchTypeModel.data(index, "name"))
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
        model: DailymotionNavModel {
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
                pageStack.push({item: Qt.resolvedUrl("DailymotionAccountsPage.qml"), immediate: true});
                break;
            case 1:
                break;
            case 2:
                pageStack.push({item: Qt.resolvedUrl("DailymotionCategoriesPage.qml"), immediate: true})
                              .model.list("/channels", {family_filter: Settings.safeSearchEnabled,
                                                        localization: Settings.locale, limit: 50});
                break;
            case 3:
                pageStack.push({item: Qt.resolvedUrl("DailymotionVideosPage.qml"), properties: {title: qsTr("My videos")},
                                immediate: true}).model.list("/me/videos", {family_filter: Settings.safeSearchEnabled,
                                                                            limit: MAX_RESULTS})
                break;
            case 4:
                pageStack.push({item: Qt.resolvedUrl("DailymotionVideosPage.qml"), properties: {title: qsTr("Latest videos")},
                                immediate: true}).model.list("/me/subscriptions",
                                                             {family_filter: Settings.safeSearchEnabled,
                                                              limit: MAX_RESULTS})
                break;
            case 5:
                pageStack.push({item: Qt.resolvedUrl("DailymotionVideosPage.qml"), properties: {title: qsTr("Favourites")},
                                immediate: true}).model.list("/me/favorites", {family_filter: Settings.safeSearchEnabled,
                                                                               limit: MAX_RESULTS})
                break;
            case 6:
                pageStack.push({item: Qt.resolvedUrl("DailymotionPlaylistsPage.qml"), immediate: true})
                .model.list("/me/playlists", {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS})
                break;
            case 7:
                pageStack.push({item: Qt.resolvedUrl("DailymotionUsersPage.qml"), properties: {title: qsTr("Subscriptions")},
                                immediate: true}).model.list("/me/following", {family_filter: Settings.safeSearchEnabled,
                                                                               limit: MAX_RESULTS})
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
        target: Dailymotion
        onCommentAdded: statusBar.showMessage(qsTr("Your comment has been added"))
        onUserSubscribed: statusBar.showMessage(qsTr("You have subscribed to") + " " + user.username)
        onUserUnsubscribed: statusBar.showMessage(qsTr("You have unsubscribed to") + " " + user.username)
        onVideoFavourited: statusBar.showMessage("'" + video.title + "' " + qsTr("added to favourites"))
        onVideoUnfavourited: statusBar.showMessage("'" + video.title + "' " + qsTr("removed from favourites"))
        onVideoAddedToPlaylist: statusBar.showMessage("'" + video.title + "' " + qsTr("added to playlist") + " '" + playlist.title + "'")
    }
}
