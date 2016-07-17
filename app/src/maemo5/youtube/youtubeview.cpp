/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "youtubeview.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "stackedwindow.h"
#include "youtube.h"
#include "youtubeaccountswindow.h"
#include "youtubecategorieswindow.h"
#include "youtubenavmodel.h"
#include "youtubeplaylist.h"
#include "youtubeplaylistswindow.h"
#include "youtubeplaylistwindow.h"
#include "youtubesearchdialog.h"
#include "youtubeuser.h"
#include "youtubeuserswindow.h"
#include "youtubeuserwindow.h"
#include "youtubevideo.h"
#include "youtubevideoswindow.h"
#include "youtubevideowindow.h"
#include <QVBoxLayout>
#include <QMaemo5InformationBox>

YouTubeView::YouTubeView(QWidget *parent) :
    QWidget(parent),
    m_model(new YouTubeNavModel(this)),
    m_view(new ListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
    connect(YouTube::instance(), SIGNAL(commentAdded(YouTubeComment*)),
            this, SLOT(onCommentAdded()));
    connect(YouTube::instance(), SIGNAL(userSubscribed(YouTubeUser*)),
            this, SLOT(onUserSubscribed(YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(userUnsubscribed(YouTubeUser*)),
            this, SLOT(onUserUnsubscribed(YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(videoFavourited(YouTubeVideo*)),
            this, SLOT(onVideoFavourited(YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoUnfavourited(YouTubeVideo*)),
            this, SLOT(onVideoUnfavourited(YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoLiked(YouTubeVideo*)),
            this, SLOT(onVideoLiked(YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoDisliked(YouTubeVideo*)),
            this, SLOT(onVideoDisliked(YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoWatchLater(YouTubeVideo*)),
            this, SLOT(onVideoWatchLater(YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoAddedToPlaylist(YouTubeVideo*, YouTubePlaylist*)),
            this, SLOT(onVideoAddedToPlaylist(YouTubeVideo*, YouTubePlaylist*)));
}

void YouTubeView::search(const QString &query, const QString &type, const QString &order) {
    QVariantMap params;
    params["q"] = query;
    params["order"] = order;
    params["maxResults"] = MAX_RESULTS;
    params["safeSearch"] = Settings::safeSearchEnabled() ? "strict" : "none";
    
    QString locale = Settings::locale();
    params["regionCode"] = locale.mid(locale.indexOf('_') + 1);
    params["relevanceLanguage"] = locale.left(locale.indexOf('_'));
    
    if (type == Resources::PLAYLIST) {
        params["type"] = "playlist";
        YouTubePlaylistsWindow *window = new YouTubePlaylistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/search", QStringList() << "snippet", QVariantMap(), params);
        window->show();
    }
    else if (type == Resources::USER) {
        params["type"] = "channel";
        YouTubeUsersWindow *window = new YouTubeUsersWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/search", QStringList() << "snippet", QVariantMap(), params);
        window->show();
    }
    else {
        params["type"] = "video";
        YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/search", QStringList() << "snippet", QVariantMap(), params);
        window->show();
    }
}

void YouTubeView::showResource(const QString &type, const QString &id) {
    if (type == Resources::PLAYLIST) {
        YouTubePlaylistWindow *window = new YouTubePlaylistWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else if (type == Resources::USER) {
        YouTubeUserWindow *window = new YouTubeUserWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else {
        YouTubeVideoWindow *window = new YouTubeVideoWindow(id, StackedWindow::currentWindow());
        window->show();
    }
}

void YouTubeView::showAccounts() {
    YouTubeAccountsWindow *window = new YouTubeAccountsWindow(StackedWindow::currentWindow());
    window->show();
}

void YouTubeView::showCategories() {
    QVariantMap filters;
    QString locale = Settings::locale();
    filters["regionCode"] = locale.mid(locale.indexOf('_') + 1);
    
    QVariantMap params;
    params["h1"] = locale.left(locale.indexOf('_'));
    
    YouTubeCategoriesWindow *window = new YouTubeCategoriesWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Categories"));
    window->list("/videoCategories", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showFavourites() {
    const QString playlistId = YouTube::relatedPlaylist("favorites");
    
    if (playlistId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any favourites"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Favourites"));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showLatestVideos() {
    QVariantMap filters;
    filters["home"] = true;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Latest videos"));
    window->list("/activities", QStringList() << "snippet" << "contentDetails", filters, params);
    window->show();
}

void YouTubeView::showLikes() {
    const QString playlistId = YouTube::relatedPlaylist("likes");
    
    if (playlistId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any likes"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Likes"));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showPlaylists() {
    QVariantMap filters;
    filters["mine"] = true;
    
    QVariantMap params;
    filters["maxResults"] = MAX_RESULTS;
    
    YouTubePlaylistsWindow *window = new YouTubePlaylistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Playlists"));
    window->list("/playlists", QStringList() << "snippet" << "contentDetails", filters, params);
    window->show();
}

void YouTubeView::showSearchDialog() {
    YouTubeSearchDialog dialog(StackedWindow::currentWindow());

    if (dialog.exec() == QDialog::Accepted) {
        const QVariantMap resource = Resources::getResourceFromUrl(dialog.query());
        
        if (resource.value("service") == Resources::YOUTUBE) {
            showResource(resource.value("type").toString(), resource.value("id").toString());
        }
        else {
            search(dialog.query(), dialog.type(), dialog.order());
        }
    }
}

void YouTubeView::showSubscriptions() {
    QVariantMap filters;
    filters["mine"] = true;
    
    QVariantMap params;
    params["sort"] = "unread";
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeUsersWindow *window = new YouTubeUsersWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Subscriptions"));
    window->list("/subscriptions", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showUploads() {
    const QString playlistId = YouTube::relatedPlaylist("uploads");
    
    if (playlistId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any videos"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("My videos"));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showWatchHistory() {
    const QString playlistId = YouTube::relatedPlaylist("watchHistory");
    
    if (playlistId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any watch history"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Watch history"));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showWatchLater() {
    const QString playlistId = YouTube::relatedPlaylist("watchLater");
    
    if (playlistId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any watch later playlist"));
        return;
    }
    
    YouTubePlaylistWindow *window = new YouTubePlaylistWindow(playlistId, StackedWindow::currentWindow());
    window->show();
}

void YouTubeView::onItemActivated(const QModelIndex &index) {
    switch (index.row()) {
    case 0:
        showAccounts();
        break;
    case 1:
        showSearchDialog();
        break;
    case 2:
        showCategories();
        break;
    case 3:
        showUploads();
        break;
    case 4:
        showFavourites();
        break;
    case 5:
        showLikes();
        break;
    case 6:
        showWatchLater();
        break;
    case 7:
        showWatchHistory();
        break;
    case 8:
        showPlaylists();
        break;
    case 9:
        showSubscriptions();
        break;
    default:
        break;
    }
}

void YouTubeView::onCommentAdded() {
    QMaemo5InformationBox::information(this, tr("Your comment has been added"));
}

void YouTubeView::onUserSubscribed(YouTubeUser *user) {
    QMaemo5InformationBox::information(this, tr("You have subscribed to %1").arg(user->username()));
}

void YouTubeView::onUserUnsubscribed(YouTubeUser *user) {
    QMaemo5InformationBox::information(this, tr("You have unsubscribed to %1").arg(user->username()));
}

void YouTubeView::onVideoFavourited(YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to favourites").arg(video->title()));
}

void YouTubeView::onVideoUnfavourited(YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' removed from favourites").arg(video->title()));
}

void YouTubeView::onVideoLiked(YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("You liked '%1'").arg(video->title()));
}

void YouTubeView::onVideoDisliked(YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("You disliked '%1'").arg(video->title()));
}

void YouTubeView::onVideoWatchLater(YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to your watch later playlist").arg(video->title()));
}

void YouTubeView::onVideoAddedToPlaylist(YouTubeVideo *video, YouTubePlaylist *playlist) {
    QMaemo5InformationBox::information(this, tr("'%1' added to playlist '%2'").arg(video->title()).arg(playlist->title()));
}
