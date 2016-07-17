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

#include "dailymotionview.h"
#include "dailymotion.h"
#include "dailymotionaccountswindow.h"
#include "dailymotioncategorieswindow.h"
#include "dailymotionnavmodel.h"
#include "dailymotionplaylist.h"
#include "dailymotionplaylistswindow.h"
#include "dailymotionplaylistwindow.h"
#include "dailymotionsearchdialog.h"
#include "dailymotionuser.h"
#include "dailymotionuserswindow.h"
#include "dailymotionuserwindow.h"
#include "dailymotionvideo.h"
#include "dailymotionvideoswindow.h"
#include "dailymotionvideowindow.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "stackedwindow.h"
#include <QVBoxLayout>
#include <QMaemo5InformationBox>

DailymotionView::DailymotionView(QWidget *parent) :
    QWidget(parent),
    m_model(new DailymotionNavModel(this)),
    m_view(new ListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
    connect(Dailymotion::instance(), SIGNAL(commentAdded(DailymotionComment*)),
            this, SLOT(onCommentAdded()));
    connect(Dailymotion::instance(), SIGNAL(userSubscribed(DailymotionUser*)),
            this, SLOT(onUserSubscribed(DailymotionUser*)));
    connect(Dailymotion::instance(), SIGNAL(userUnsubscribed(DailymotionUser*)),
            this, SLOT(onUserUnsubscribed(DailymotionUser*)));
    connect(Dailymotion::instance(), SIGNAL(videoFavourited(DailymotionVideo*)),
            this, SLOT(onVideoFavourited(DailymotionVideo*)));
    connect(Dailymotion::instance(), SIGNAL(videoUnfavourited(DailymotionVideo*)),
            this, SLOT(onVideoUnfavourited(DailymotionVideo*)));
    connect(Dailymotion::instance(), SIGNAL(videoAddedToPlaylist(DailymotionVideo*, DailymotionPlaylist*)),
            this, SLOT(onVideoAddedToPlaylist(DailymotionVideo*, DailymotionPlaylist*)));
}

void DailymotionView::search(const QString &query, const QString &type, const QString &order) {
    QVariantMap filters;
    filters["search"] = query;
    filters["sort"] = order;
    filters["limit"] = MAX_RESULTS;
    filters["family_filter"] = Settings::safeSearchEnabled();
    filters["localization"] = Settings::locale();
    
    if (type == Resources::PLAYLIST) {
        DailymotionPlaylistsWindow *window = new DailymotionPlaylistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/playlists", filters);
        window->show();
    }
    else if (type == Resources::USER) {
        DailymotionUsersWindow *window = new DailymotionUsersWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/users", filters);
        window->show();
    }
    else {
        DailymotionVideosWindow *window = new DailymotionVideosWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->list("/videos", filters);
        window->show();
    }
}

void DailymotionView::showResource(const QString &type, const QString &id) {
    if (type == Resources::PLAYLIST) {
        DailymotionPlaylistWindow *window = new DailymotionPlaylistWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else if (type == Resources::USER) {
        DailymotionUserWindow *window = new DailymotionUserWindow(id, StackedWindow::currentWindow());
        window->show();
    }
    else {
        DailymotionVideoWindow *window = new DailymotionVideoWindow(id, StackedWindow::currentWindow());
        window->show();
    }
}

void DailymotionView::showAccounts() {
    DailymotionAccountsWindow *window = new DailymotionAccountsWindow(StackedWindow::currentWindow());
    window->show();
}

void DailymotionView::showCategories() {
    QVariantMap filters;
    filters["family_filter"] = Settings::safeSearchEnabled();
    filters["localization"] = Settings::locale();
    filters["limit"] = 50;
    
    DailymotionCategoriesWindow *window = new DailymotionCategoriesWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Categories"));
    window->list("/channels", filters);
    window->show();
}

void DailymotionView::showFavourites() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    DailymotionVideosWindow *window = new DailymotionVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Favourites"));
    window->list("/me/favorites", filters);
    window->show();
}

void DailymotionView::showLatestVideos() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    DailymotionVideosWindow *window = new DailymotionVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Latest videos"));
    window->list("/me/subscriptions", filters);
    window->show();
}

void DailymotionView::showPlaylists() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    DailymotionPlaylistsWindow *window = new DailymotionPlaylistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Playlists"));
    window->list("/me/playlists", filters);
    window->show();
}

void DailymotionView::showSearchDialog() {
    DailymotionSearchDialog dialog(StackedWindow::currentWindow());

    if (dialog.exec() == QDialog::Accepted) {
        const QVariantMap resource = Resources::getResourceFromUrl(dialog.query());
        
        if (resource.value("service") == Resources::DAILYMOTION) {
            showResource(resource.value("type").toString(), resource.value("id").toString());
        }
        else {
            search(dialog.query(), dialog.type(), dialog.order());
        }
    }
}

void DailymotionView::showSubscriptions() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    DailymotionUsersWindow *window = new DailymotionUsersWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("Subscriptions"));
    window->list("/me/following", filters);
    window->show();
}

void DailymotionView::showUploads() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    
    DailymotionVideosWindow *window = new DailymotionVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(tr("My videos"));
    window->list("/me/videos", filters);
    window->show();
}

void DailymotionView::onItemActivated(const QModelIndex &index) {
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
        showLatestVideos();
        break;
    case 5:
        showFavourites();
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

void DailymotionView::onCommentAdded() {
    QMaemo5InformationBox::information(this, tr("Your comment has been added"));
}

void DailymotionView::onUserSubscribed(DailymotionUser *user) {
    QMaemo5InformationBox::information(this, tr("You have subscribed to %1").arg(user->username()));
}

void DailymotionView::onUserUnsubscribed(DailymotionUser *user) {
    QMaemo5InformationBox::information(this, tr("You have unsubscribed to %1").arg(user->username()));
}

void DailymotionView::onVideoFavourited(DailymotionVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' added to favourites").arg(video->title()));
}

void DailymotionView::onVideoUnfavourited(DailymotionVideo *video) {
    QMaemo5InformationBox::information(this, tr("'%1' removed from favourites").arg(video->title()));
}

void DailymotionView::onVideoAddedToPlaylist(DailymotionVideo *video, DailymotionPlaylist *playlist) {
    QMaemo5InformationBox::information(this, tr("'%1' added to playlist '%2'").arg(video->title()).arg(playlist->title()));
}
