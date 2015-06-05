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
        var params = {};
        params["q"] = query;
        params["order"] = order;
        params["maxResults"] = MAX_RESULTS;
        params["safeSearch"] = Settings.safeSearchEnabled ? "strict" : "none";

        if (type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("YouTubePlaylistsPage.qml");
            params["type"] = "playlist";
        }
        else if (type == Resources.USER) {
            url = Qt.resolvedUrl("YouTubeUsersPage.qml");
            params["type"] = "channel";
        }
        else {
            url = Qt.resolvedUrl("YouTubeVideosPage.qml");
            params["type"] = "video";
        }

        view.currentIndex = 1;
        
        if (pageStack.depth > 0) {
            pageStack.clear();
        }
            
        pageStack.push({item: url, properties: {title: qsTr("Search") + " ('" + query + "')"}, immediate: true})
                       .model.list("/search", ["snippet"], filters, params);
        return true;
    }

    function showResource(resource) {
        var url;

        if (resource.type == Resources.PLAYLIST) {
            url = Qt.resolvedUrl("YouTubePlaylistPage.qml");
        }
        else if (resource.type == Resources.USER) {
            url = Qt.resolvedUrl("YouTubeUserPage.qml");
        }
        else {
            url = Qt.resolvedUrl("YouTubeVideoPage.qml");
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
            currentIndex: serviceModel.match("value", Resources.YOUTUBE)
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
            
            model: YouTubeSearchTypeModel {
                id: searchTypeModel
            }
            textRole: "name"
            currentIndex: searchTypeModel.match("value", Settings.defaultSearchType(Resources.YOUTUBE))
            onActivated: Settings.setDefaultSearchType(Resources.YOUTUBE, searchTypeModel.data(index, "value"))
        }
        
        Label {
            text: qsTr("Order by")
        }
        
        ComboBox {
            id: searchOrderSelector
            
            model: YouTubeSearchOrderModel {
                id: searchOrderModel
            }
            textRole: "name"
            currentIndex: searchOrderModel.match("value", Settings.defaultSearchOrder(Resources.YOUTUBE))
            onActivated: Settings.setDefaultSearchOrder(Resources.YOUTUBE, searchOrderModel.data(index, "value"))
        }
        
        TextField {
            id: searchField
            
            Layout.minimumWidth: 300
            placeholderText: qsTr("Search")
            validator: RegExpValidator {
                regExp: /^.+/
            }
            onAccepted: {
                root.search(text, searchTypeModel.data(searchTypeSelector.currentIndex, "value"),
                            searchOrderModel.data(searchOrderSelector.currentIndex, "value"));
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
        model: YouTubeNavModel {
            id: navModel
        }
        delegate: LabelDelegate {
            text: display
        }
        currentIndex: 1
        onCurrentIndexChanged: {
            if (pageStack.depth > 0) {
                pageStack.clear();
            }
            
            switch (currentIndex) {
            case 0:
                pageStack.push({item: Qt.resolvedUrl("YouTubeAccountsPage.qml"), immediate: true});
                break;
            case 1:
                break;
            case 2:
                pageStack.push({item: Qt.resolvedUrl("YouTubeCategoriesPage.qml"), immediate: true})
                              .model.list("/videoCategories", ["snippet"], {regionCode: Qt.locale().name.split("_")[1]},
                                          {h1: Qt.locale().name});
                break;
            case 3:
                pageStack.push({item: Qt.resolvedUrl("YouTubeVideosPage.qml"), properties: {title: qsTr("My videos")},
                                immediate: true}).model.list("/playlistItems", ["snippet"],
                                {playlistId: YouTube.relatedPlaylist("uploads")}, {maxResults: MAX_RESULTS});
                break;
            case 4:
                pageStack.push({item: Qt.resolvedUrl("YouTubeVideosPage.qml"), properties: {title: qsTr("Favourites")},
                                immediate: true}).model.list("/playlistItems", ["snippet"],
                                {playlistId: YouTube.relatedPlaylist("favorites")}, {maxResults: MAX_RESULTS});
                break;
            case 5:
                pageStack.push({item: Qt.resolvedUrl("YouTubeVideosPage.qml"), properties: {title: qsTr("Likes")},
                                immediate: true}).model.list("/playlistItems", ["snippet"],
                                {playlistId: YouTube.relatedPlaylist("likes")}, {maxResults: MAX_RESULTS});
                break;
            case 6:
                pageStack.push({item: Qt.resolvedUrl("YouTubePlaylistPage.qml"), immediate: true})
                .load(YouTube.relatedPlaylists("watchLater"));
                break;
            case 7:
                pageStack.push({item: Qt.resolvedUrl("YouTubeVideosPage.qml"), properties: {title: qsTr("Watch history")},
                                immediate: true}).model.list("/playlistItems", ["snippet"],
                                {playlistId: YouTube.relatedPlaylist("watchHistory")}, {maxResults: MAX_RESULTS});
                break;
            case 8:
                pageStack.push({item: Qt.resolvedUrl("YouTubePlaylistsPage.qml"), immediate: true})
                .model.list("/playlists", ["snippet", "contentDetails"], {mine: true}, {maxResults: MAX_RESULTS})
                break;
            case 9:
                pageStack.push({item: Qt.resolvedUrl("YouTubeUsersPage.qml"), properties: {title: qsTr("Subscriptions")},
                                immediate: true}).model.list("/subscriptions", ["snippet"], {mine: true},
                                {sort: "unread", maxResults: MAX_RESULTS});
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
        target: YouTube
        onCommentAdded: statusBar.showMessage(qsTr("Your comment has been added"))
        onUserSubscribed: statusBar.showMessage(qsTr("You have subscribed to") + " " + user.username)
        onUserUnsubscribed: statusBar.showMessage(qsTr("You have unsubscribed to") + " " + user.username)
        onVideoFavourited: statusBar.showMessage("'" + video.title + "' " + qsTr("added to favourites"))
        onVideoUnfavourited: statusBar.showMessage("'" + video.title + "' " + qsTr("removed from favourites"))
        onVideoLiked: statusBar.showMessage(qsTr("You liked") + " '" + video.title + "'")
        onVideoDisliked: statusBar.showMessage(qsTr("You disliked") + " '" + video.title + "'")
        onVideoWatchLater: statusBar.showMessage("'" + video.title + "' " + qsTr("added to your watch later playlist"))
        onVideoAddedToPlaylist: statusBar.showMessage("'" + video.title + "' " + qsTr("added to playlist") + " '" + playlist.title + "'")
    }
}
