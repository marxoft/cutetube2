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
import com.nokia.symbian 1.1
import cuteTube 2.0
import "dailymotion"
import "plugins"
import "vimeo"
import "youtube"

MyPage {
    id: root

    function search(service, query, type, order) {
        setService(service);
        appWindow.pageStack.pop(null, true);
        viewLoader.item.search(query, type, order);
    }

    function setService(service) {
        if (!service) {
            return;
        }

        Settings.currentService = service;
        
        if (service == Resources.YOUTUBE) {
            viewLoader.sourceComponent = youtubeView;
        }
        else if (service == Resources.DAILYMOTION) {
            viewLoader.sourceComponent = dailymotionView;
        }
        else if (service == Resources.VIMEO) {
            viewLoader.sourceComponent = vimeoView;
        }
        else {
            viewLoader.sourceComponent = pluginView;
        }
    }

    function showResource(resource) {
        if (!resource.service) {
            return false;
        }

        setService(resource.service);
        appWindow.pageStack.pop(null, true);
        MainWindow.raise();
        viewLoader.item.showResource(resource);
        return true;
    }

    function showResourceFromUrl(url) {
        return showResource(Resources.getResourceFromUrl(url));
    }
    
    title: Settings.currentService ? serviceModel.data(serviceModel.match("value", Settings.currentService), "name")
                                   : "cuteTube2"
    tools: ToolBarLayout {

        BackToolButton {}

        MyToolButton {
            iconSource: "toolbar-settings"
            toolTipText: qsTr("Settings")
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("SettingsPage.qml"))
        }

        MyToolButton {
            iconSource: "images/download.png"
            toolTipText: qsTr("Transfers")
            onClicked: appWindow.pageStack.push(Qt.resolvedUrl("TransfersPage.qml"))
        }

        MyToolButton {
            iconSource: "toolbar-list"
            toolTipText: qsTr("Service")
            onClicked: {
                dialogLoader.sourceComponent = serviceDialog;
                dialogLoader.item.open();
            }
        }
    }

    ServiceModel {
        id: serviceModel
    }
    
    Loader {
        id: viewLoader
        
        anchors.fill: parent
    }
    
    Loader {
        id: dialogLoader        
    }
    
    Component {
        id: youtubeView
        
        MyListView {

            function search(query, type, order) {
                var url;
                var filters = {};
                var params = {};
                params["q"] = query;
                params["order"] = order;
                params["maxResults"] = MAX_RESULTS;
                params["safeSearch"] = Settings.safeSearchEnabled ? "strict" : "none";
                params["regionCode"] = Settings.locale.split("_")[1];
                params["relevanceLanguage"] = Settings.locale.split("_")[0];

                if (type == Resources.PLAYLIST) {
                    url = Qt.resolvedUrl("youtube/YouTubePlaylistsPage.qml");
                    params["type"] = "playlist";
                }
                else if (type == Resources.USER) {
                    url = Qt.resolvedUrl("youtube/YouTubeUsersPage.qml");
                    params["type"] = "channel";
                }
                else {
                    url = Qt.resolvedUrl("youtube/YouTubeVideosPage.qml");
                    params["type"] = "video";
                }

                appWindow.pageStack.push(url, {title: qsTr("Search") + " ('" + query + "')"})
                                        .model.list("/search", ["snippet"], filters, params);
            }

            function showResource(resource) {
                var url;

                if (resource.type == Resources.PLAYLIST) {
                    url = Qt.resolvedUrl("youtube/YouTubePlaylistPage.qml");
                }
                else if (resource.type == Resources.USER) {
                    url = Qt.resolvedUrl("youtube/YouTubeUserPage.qml");
                }
                else {
                    url = Qt.resolvedUrl("youtube/YouTubeVideoPage.qml");
                }

                appWindow.pageStack.push(url).load(resource.id);
            }

            anchors.fill: parent
            model: YouTubeNavModel {}
            delegate: DrillDownDelegate {
                onClicked: {
                    switch (index) {
                    case 0:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeAccountsPage.qml"));
                        break;
                    case 1: {
                        dialogLoader.sourceComponent = youtubeSearchDialog;
                        dialogLoader.item.open();
                        break;
                    }
                    case 2:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeCategoriesPage.qml"))
                        .model.list("/videoCategories", ["snippet"], {regionCode: Settings.locale.split("_")[1]},
                                    {h1: Settings.locale});
                        break;
                    case 3:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeVideosPage.qml"), {title: qsTr("My videos")})
                        .model.list("/playlistItems", ["snippet"], {playlistId: YouTube.relatedPlaylist("uploads")},
                                    {maxResults: MAX_RESULTS});
                        break;
                    case 4:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeVideosPage.qml"), {title: qsTr("Favourites")})
                        .model.list("/playlistItems", ["snippet"], {playlistId: YouTube.relatedPlaylist("favorites")},
                                    {maxResults: MAX_RESULTS});
                        break;
                    case 5:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeVideosPage.qml"), {title: qsTr("Likes")})
                        .model.list("/playlistItems", ["snippet"], {playlistId: YouTube.relatedPlaylist("likes")},
                                    {maxResults: MAX_RESULTS});
                        break;
                    case 6:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubePlaylistPage.qml"))
                        .load(YouTube.relatedPlaylist("watchLater"));
                        break;
                    case 7:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeVideosPage.qml"), {title: qsTr("Watch history")})
                        .model.list("/playlistItems", ["snippet"], {playlistId: YouTube.relatedPlaylist("watchHistory")},
                                    {maxResults: MAX_RESULTS});
                        break;
                    case 8:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubePlaylistsPage.qml"))
                        .model.list("/playlists", ["snippet", "contentDetails"], {mine: true}, {maxResults: MAX_RESULTS});
                        break;
                    case 9:
                        appWindow.pageStack.push(Qt.resolvedUrl("youtube/YouTubeUsersPage.qml"), {title: qsTr("Subscriptions")})
                        .model.list("/subscriptions", ["snippet"], {mine: true}, {sort: "unread", maxResults: MAX_RESULTS});
                        break;
                    }
                }
            }

            Connections {
                target: YouTube
                onCommentAdded: infoBanner.showMessage(qsTr("Your comment has been added"))
                onUserSubscribed: infoBanner.showMessage(qsTr("You have subscribed to") + " " + user.username)
                onUserUnsubscribed: infoBanner.showMessage(qsTr("You have unsubscribed to") + " " + user.username)
                onVideoFavourited: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to favourites"))
                onVideoUnfavourited: infoBanner.showMessage("'" + video.title + "' " + qsTr("removed from favourites"))
                onVideoLiked: infoBanner.showMessage(qsTr("You liked") + " '" + video.title + "'")
                onVideoDisliked: infoBanner.showMessage(qsTr("You disliked") + " '" + video.title + "'")
                onVideoWatchLater: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to your watch later playlist"))
                onVideoAddedToPlaylist: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to playlist") + " '" + playlist.title + "'")
            }
        }
    }
    
    Component {
        id: dailymotionView

        MyListView {

            function search(query, type, order) {
                var filters = {};
                filters["search"] = query;
                filters["sort"] = order;
                filters["limit"] = MAX_RESULTS;
                filters["family_filter"] = Settings.safeSearchEnabled;
                filters["localization"] = Settings.locale;

                if (type == Resources.PLAYLIST) {
                    appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionPlaylistsPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.list("/playlists", filters);
                }
                else if (type == Resources.USER) {
                    appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionUsersPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.list("/users", filters);
                }
                else {
                    appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionVideosPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.list("/videos", filters);
                }
            }

            function showResource(resource) {
                var url;

                if (resource.type == Resources.PLAYLIST) {
                    url = Qt.resolvedUrl("dailymotion/DailymotionPlaylistPage.qml");
                }
                else if (resource.type == Resources.USER) {
                    url = Qt.resolvedUrl("dailymotion/DailymotionUserPage.qml");
                }
                else {
                    url = Qt.resolvedUrl("dailymotion/DailymotionVideoPage.qml");
                }

                appWindow.pageStack.push(url).load(resource.id);
            }

            anchors.fill: parent
            model: DailymotionNavModel {}
            delegate: DrillDownDelegate {
                onClicked: {
                    switch (index) {
                    case 0:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionAccountsPage.qml"));
                        break;
                    case 1: {
                        dialogLoader.sourceComponent = dailymotionSearchDialog;
                        dialogLoader.item.open();
                        break;
                    }
                    case 2:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionCategoriesPage.qml"))
                        .model.list("/channels", {family_filter: Settings.safeSearchEnabled,
                                                  localization: Settings.locale, limit: 50});
                        break;
                    case 3:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionVideosPage.qml"), {title: qsTr("My videos")})
                        .model.list("/me/videos", {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS});
                        break;
                    case 4:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionVideosPage.qml"), {title: qsTr("Latest videos")})
                        .model.list("/me/subscriptions", {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS});
                        break;
                    case 5:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionVideosPage.qml"), {title: qsTr("Favourites")})
                        .model.list("/me/favorites", {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS});
                        break;
                    case 6:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionPlaylistsPage.qml"), {title: qsTr("Playlists")})
                        .model.list("/me/playlists", {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS});
                        break;
                    case 7:
                        appWindow.pageStack.push(Qt.resolvedUrl("dailymotion/DailymotionUsersPage.qml"), {title: qsTr("Subscriptions")})
                        .model.list("/me/following", {family_filter: Settings.safeSearchEnabled, limit: MAX_RESULTS});
                        break;
                    }
                }
            }

            Connections {
                target: Dailymotion
                onCommentAdded: infoBanner.showMessage(qsTr("Your comment has been added"))
                onUserSubscribed: infoBanner.showMessage(qsTr("You have subscribed to") + " " + user.username)
                onUserUnsubscribed: infoBanner.showMessage(qsTr("You have unsubscribed to") + " " + user.username)
                onVideoFavourited: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to favourites"))
                onVideoUnfavourited: infoBanner.showMessage("'" + video.title + "' " + qsTr("removed from favourites"))
                onVideoAddedToPlaylist: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to playlist") + " '" + playlist.title + "'")
            }
        }        
    }
    
    Component {
        id: vimeoView        
        
        MyListView {

            function search(query, type, order) {
                var filters = {};
                filters["query"] = query;
                filters["sort"] = order;
                filters["per_page"] = MAX_RESULTS;

                if (type == Resources.USER) {
                    appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoUsersPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.list("/users", filters);
                }
                else {
                    appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoVideosPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.list("/videos", filters);
                }
            }

            function showResource(resource) {
                var url;

                if (resource.type == Resources.PLAYLIST) {
                    url = Qt.resolvedUrl("vimeo/VimeoPlaylistPage.qml");
                }
                else if (resource.type == Resources.USER) {
                    url = Qt.resolvedUrl("vimeo/VimeoUserPage.qml");
                }
                else {
                    url = Qt.resolvedUrl("vimeo/VimeoVideoPage.qml");
                }

                appWindow.pageStack.push(url).load(resource.id);
            }

            anchors.fill: parent
            model: VimeoNavModel {}
            delegate: DrillDownDelegate {
                onClicked: {
                    switch (index) {
                    case 0:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoAccountsPage.qml"));
                        break;
                    case 1: {
                        dialogLoader.sourceComponent = vimeoSearchDialog;
                        dialogLoader.item.open();
                        break;
                    }
                    case 2:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoCategoriesPage.qml"))
                        .model.list("/categories", {per_page: 50});
                        break;
                    case 2:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoVideosPage.qml"), {title: qsTr("My videos")})
                        .model.list("/me/videos", {per_page: MAX_RESULTS});
                        break;
                    case 3:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoVideosPage.qml"), {title: qsTr("Latest videos")})
                        .model.list("/me/feed", {per_page: MAX_RESULTS});
                        break;
                    case 4:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoVideosPage.qml"), {title: qsTr("Likes")})
                        .model.list("/me/likes", {per_page: MAX_RESULTS});
                        break;
                    case 5:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoVideosPage.qml"), {title: qsTr("Watch later")})
                        .model.list("/me/watchlater", {per_page: MAX_RESULTS});
                        break;
                    case 6:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoPlaylistsPage.qml"), {title: qsTr("Albums")})
                        .model.list("/me/albums", {per_page: MAX_RESULTS});
                        break;
                    case 7:
                        appWindow.pageStack.push(Qt.resolvedUrl("vimeo/VimeoUsersPage.qml"), {title: qsTr("Subscriptions")})
                        .model.list("/me/following", {per_page: MAX_RESULTS});
                        break;
                    }
                }
            }

            Connections {
                target: Vimeo
                onCommentAdded: infoBanner.showMessage(qsTr("Your comment has been added"))
                onUserSubscribed: infoBanner.showMessage(qsTr("You have subscribed to") + " " + user.username)
                onUserUnsubscribed: infoBanner.showMessage(qsTr("You have unsubscribed to") + " " + user.username)
                onVideoFavourited: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to likes"))
                onVideoUnfavourited: infoBanner.showMessage("'" + video.title + "' " + qsTr("removed from likes"))
                onVideoWatchLater: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to your watch later album"))
                onVideoAddedToPlaylist: infoBanner.showMessage("'" + video.title + "' " + qsTr("added to album") + " '" + playlist.title + "'")
            }
        }        
    }
    
    Component {
        id: pluginView
        
        MyListView {

            function search(query, type, order) {
                if (type == Resources.PLAYLIST) {
                    appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginPlaylistsPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.search(query, order);
                }
                else if (type == Resources.USER) {
                    appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginUsersPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.search(query, order);
                }
                else {
                    appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginVideosPage.qml"),
                                             {title: qsTr("Search") + " ('" + query + "')"}).model.search(query, order);
                }
            }

            anchors.fill: parent
            model: PluginNavModel {
                service: Settings.currentService
            }
            delegate: DrillDownDelegate {
                text: name
                onClicked: {
                    if (!value) {
                        dialogLoader.sourceComponent = pluginSearchDialog;
                        dialogLoader.item.open();
                    }
                    else if (value.type == Resources.CATEGORY) {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginCategoriesPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                    else if (value.type == Resources.PLAYLIST) {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginPlaylistsPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                    else if (value.type == Resources.USER) {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginUsersPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                    else {
                        appWindow.pageStack.push(Qt.resolvedUrl("plugins/PluginVideosPage.qml"),
                                                 {title: value.name}).model.list(value.id);
                    }
                }
            }
        }
    }

    Component {
        id: serviceDialog

        ValueDialog {
            titleText: qsTr("Service")
            model: serviceModel
            focusItem: viewLoader.item
            value: Settings.currentService
            onValueChanged: root.setService(value)
        }
    }

    Component {
        id: youtubeSearchDialog

        YouTubeSearchDialog {
            focusItem: root.status == PageStatus.Active ? viewLoader.item : null
        }
    }

    Component {
        id: dailymotionSearchDialog

        DailymotionSearchDialog {
            focusItem: root.status == PageStatus.Active ? viewLoader.item : null
        }
    }

    Component {
        id: vimeoSearchDialog

        VimeoSearchDialog {
            focusItem: root.status == PageStatus.Active ? viewLoader.item : null
        }
    }

    Component {
        id: pluginSearchDialog

        PluginSearchDialog {
            focusItem: root.status == PageStatus.Active ? viewLoader.item : null
        }
    }
}
