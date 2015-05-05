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

#include "vimeoview.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "stackedwindow.h"
#include "vimeo.h"
#include "vimeoaccountswindow.h"
#include "vimeonavmodel.h"
#include "vimeoplaylist.h"
#include "vimeoplaylistswindow.h"
#include "vimeoplaylistwindow.h"
#include "vimeosearchdialog.h"
#include "vimeouser.h"
#include "vimeouserswindow.h"
#include "vimeouserwindow.h"
#include "vimeovideo.h"
#include "vimeovideoswindow.h"
#include "vimeovideowindow.h"
#include <QVBoxLayout>
#include <QMaemo5InformationBox>

VimeoView::VimeoView(QWidget *parent) :
    QWidget(parent),
    m_model(new VimeoNavModel(this)),
    m_view(new ListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    m_layout->addWidget(m_view);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
    connect(Vimeo::instance(), SIGNAL(commentAdded(const VimeoComment*)),
            this, SLOT(onCommentAdded()));
    connect(Vimeo::instance(), SIGNAL(userSubscribed(const VimeoUser*)),
            this, SLOT(onUserSubscribed(const VimeoUser*)));
    connect(Vimeo::instance(), SIGNAL(userUnsubscribed(const VimeoUser*)),
            this, SLOT(onUserUnsubscribed(const VimeoUser*)));
    connect(Vimeo::instance(), SIGNAL(videoFavourited(const VimeoVideo*)),
            this, SLOT(onVideoFavourited(const VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoUnfavourited(const VimeoVideo*)),
            this, SLOT(onVideoUnfavourited(const VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoWatchLater(const VimeoVideo*)),
            this, SLOT(onVideoWatchLater(const VimeoVideo*)));
    connect(Vimeo::instance(), SIGNAL(videoAddedToPlaylist(const VimeoVideo*, const VimeoPlaylist*)),
            this, SLOT(onVideoAddedToPlaylist(const VimeoVideo*, const VimeoPlaylist*)));
}

void VimeoView::search(const QString &query, const QString &type, const QString &order) {
    QVariantMap filters;
    filters["query"] = query;
    filters["sort"] = order;
    filters["per_page"] = MAX_RESULTS;
    
    if (type == Resources::USER) {
        VimeoUsersWindow *window = new VimeoUsersWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/users", filters);
        window->show();
    }
    else {
        VimeoVideosWindow *window = new VimeoVideosWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/videos", filters);
        window->show();
    }
}

void VimeoView::showResource(const QString &type, const QString &id) {
    if (type == Resources::PLAYLIST) {
        VimeoPlaylistWindow *window = new VimeoPlaylistWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else if (type == Resources::USER) {
        VimeoUserWindow *window = new VimeoUserWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else {
        VimeoVideoWindow *window = new VimeoVideoWindow(id, StackedWindow::currentWindow());
        window->show();
    }
}

void VimeoView::showAccounts() {
    VimeoAccountsWindow *window = new VimeoAccountsWindow(StackedWindow::currentWindow());
    window->show();
}

void VimeoView::showFavourites() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Likes"));
    window->list("/me/likes", filters);
    window->show();
}

void VimeoView::showLatestVideos() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Latest videos"));
    window->list("/me/feed", filters);
    window->show();
}

void VimeoView::showPlaylists() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoPlaylistsWindow *window = new VimeoPlaylistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Albums"));
    window->list("/me/albums", filters);
    window->show();
}

void VimeoView::showSearchDialog() {
    VimeoSearchDialog *dialog = new VimeoSearchDialog(StackedWindow::currentWindow());
    dialog->open();
}

void VimeoView::showSubscriptions() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoUsersWindow *window = new VimeoUsersWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Subscriptions"));
    window->list("/me/following", filters);
    window->show();
}

void VimeoView::showUploads() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("My videos"));
    window->list("/me/videos", filters);
    window->show();
}

void VimeoView::showWatchLater() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    
    VimeoVideosWindow *window = new VimeoVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Watch later"));
    window->list("/me/watchlater", filters);
    window->show();
}

void VimeoView::onItemActivated(const QModelIndex &index) {
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
        showLatestVideos();
        break;
    case 4:
        showFavourites();
        break;
    case 5:
        showWatchLater();
        break;
    case 6:
        showPlaylists();
        break;
    case 7:
        showSubscriptions();
        break;
    default:
        break;
    }
}

void VimeoView::onCommentAdded() {
    QMaemo5InformationBox::information(this, tr("Your comment has been added"));
}

void VimeoView::onUserSubscribed(const VimeoUser *user) {
    QMaemo5InformationBox::information(this, tr("You have subscribed to %1").arg(user->username()));
}

void VimeoView::onUserUnsubscribed(const VimeoUser *user) {
    QMaemo5InformationBox::information(this, tr("You have unsubscribed to %1").arg(user->username()));
}

void VimeoView::onVideoFavourited(const VimeoVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to likes").arg(video->title()));
}

void VimeoView::onVideoUnfavourited(const VimeoVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' removed from likes").arg(video->title()));
}

void VimeoView::onVideoWatchLater(const VimeoVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to your watch later playlist").arg(video->title()));
}

void VimeoView::onVideoAddedToPlaylist(const VimeoVideo *video, const VimeoPlaylist *playlist) {
    QMaemo5InformationBox::information(this, tr("'%1' added to album '%2'").arg(video->title()).arg(playlist->title()));
}
