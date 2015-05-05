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

#include "youtubeview.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "stackedwindow.h"
#include "youtube.h"
#include "youtubeaccountswindow.h"
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
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
    connect(YouTube::instance(), SIGNAL(commentAdded(const YouTubeComment*)),
            this, SLOT(onCommentAdded()));
    connect(YouTube::instance(), SIGNAL(userSubscribed(const YouTubeUser*)),
            this, SLOT(onUserSubscribed(const YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(userUnsubscribed(const YouTubeUser*)),
            this, SLOT(onUserUnsubscribed(const YouTubeUser*)));
    connect(YouTube::instance(), SIGNAL(videoFavourited(const YouTubeVideo*)),
            this, SLOT(onVideoFavourited(const YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoUnfavourited(const YouTubeVideo*)),
            this, SLOT(onVideoUnfavourited(const YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoLiked(const YouTubeVideo*)),
            this, SLOT(onVideoLiked(const YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoDisliked(const YouTubeVideo*)),
            this, SLOT(onVideoDisliked(const YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoWatchLater(const YouTubeVideo*)),
            this, SLOT(onVideoWatchLater(const YouTubeVideo*)));
    connect(YouTube::instance(), SIGNAL(videoAddedToPlaylist(const YouTubeVideo*, const YouTubePlaylist*)),
            this, SLOT(onVideoAddedToPlaylist(const YouTubeVideo*, const YouTubePlaylist*)));
}

void YouTubeView::search(const QString &query, const QString &type, const QString &order) {
    QVariantMap filters;
    filters["q"] = query;
    filters["type"] = type;
    filters["order"] = order;
    filters["maxResults"] = MAX_RESULTS;
    filters["safeSearch"] = Settings::instance()->safeSearchEnabled() ? "strict" : "none";
    
    if (type == Resources::PLAYLIST) {
        YouTubePlaylistsWindow *window = new YouTubePlaylistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/search", QStringList() << "snippet", QVariantMap(), filters);
        window->show();
    }
    else if (type == Resources::USER) {
        YouTubeUsersWindow *window = new YouTubeUsersWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/search", QStringList() << "snippet", QVariantMap(), filters);
        window->show();
    }
    else {
        YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/search", QStringList() << "snippet", QVariantMap(), filters);
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

void YouTubeView::showFavourites() {
    QString playlistId = YouTube::instance()->relatedPlaylist("favorites");
    
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
    QString playlistId = YouTube::instance()->relatedPlaylist("likes");
    
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

void YouTubeView::showRecommended() {
    QString playlistId = YouTube::instance()->relatedPlaylist("recommended");
    
    if (playlistId.isEmpty()) {
        QMaemo5InformationBox::information(this, tr("This channel does not have any recommended videos"));
        return;
    }
    
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeVideosWindow *window = new YouTubeVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Recommended"));
    window->list("/playlistItems", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showSearchDialog() {
    YouTubeSearchDialog *dialog = new YouTubeSearchDialog(StackedWindow::currentWindow());
    dialog->open();
}

void YouTubeView::showSubscriptions() {
    QVariantMap filters;
    filters["mine"] = true;
    
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    
    YouTubeUsersWindow *window = new YouTubeUsersWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Subscriptions"));
    window->list("/subscriptions", QStringList() << "snippet", filters, params);
    window->show();
}

void YouTubeView::showUploads() {
    QString playlistId = YouTube::instance()->relatedPlaylist("uploads");
    
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
    QString playlistId = YouTube::instance()->relatedPlaylist("watchHistory");
    
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
    QString playlistId = YouTube::instance()->relatedPlaylist("watchLater");
    
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
        showUploads();
        break;
    case 3:
        showFavourites();
        break;
    case 4:
        showLikes();
        break;
    case 5:
        showWatchLater();
        break;
    case 6:
        showWatchHistory();
        break;
    case 7:
        showPlaylists();
        break;
    case 8:
        showSubscriptions();
        break;
    default:
        break;
    }
}

void YouTubeView::onCommentAdded() {
    QMaemo5InformationBox::information(this, tr("Your comment has been added"));
}

void YouTubeView::onUserSubscribed(const YouTubeUser *user) {
    QMaemo5InformationBox::information(this, tr("You have subscribed to %1").arg(user->username()));
}

void YouTubeView::onUserUnsubscribed(const YouTubeUser *user) {
    QMaemo5InformationBox::information(this, tr("You have unsubscribed to %1").arg(user->username()));
}

void YouTubeView::onVideoFavourited(const YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to favourites").arg(video->title()));
}

void YouTubeView::onVideoUnfavourited(const YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' removed from favourites").arg(video->title()));
}

void YouTubeView::onVideoLiked(const YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("You liked '%1'").arg(video->title()));
}

void YouTubeView::onVideoDisliked(const YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("You disliked '%1'").arg(video->title()));
}

void YouTubeView::onVideoWatchLater(const YouTubeVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to your watch later playlist").arg(video->title()));
}

void YouTubeView::onVideoAddedToPlaylist(const YouTubeVideo *video, const YouTubePlaylist *playlist) {
    QMaemo5InformationBox::information(this, tr("'%1' added to playlist '%2'").arg(video->title()).arg(playlist->title()));
}
