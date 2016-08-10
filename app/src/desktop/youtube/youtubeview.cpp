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
#include "resources.h"
#include "settings.h"
#include "youtube.h"
#include "youtubecategoriespage.h"
#include "youtubenavmodel.h"
#include "youtubeplaylistspage.h"
#include "youtubesearchdialog.h"
#include "youtubeuserspage.h"
#include "youtubevideospage.h"
#include <QListView>
#include <QMessageBox>
#include <QStackedWidget>
#include <QSplitter>
#include <QVBoxLayout>

YouTubeView::YouTubeView(QWidget *parent) :
    PageStack(parent),
    m_model(new YouTubeNavModel(this)),
    m_view(new QListView(this)),
    m_stack(new QStackedWidget(this)),
    m_splitter(new QSplitter(Qt::Horizontal, this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle("YouTube");
    
    m_view->setModel(m_model);

    m_splitter->addWidget(m_view);
    m_splitter->addWidget(m_stack);
    m_splitter->setStretchFactor(1, 1);

    m_layout->addWidget(m_splitter);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentPage(QModelIndex)));
}

int YouTubeView::count() const {
    return m_stack->count();
}

Page* YouTubeView::currentPage() const {
    return qobject_cast<Page*>(m_stack->currentWidget());
}

void YouTubeView::push(Page *page) {
    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit countChanged(count());
    emit currentChanged(page);
}

void YouTubeView::replace(Page *page) {
    if (QWidget *w = m_stack->currentWidget()) {
        m_stack->removeWidget(w);
        w->deleteLater();
    }

    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit currentChanged(page);
}

void YouTubeView::pop() {
    if (m_stack->count() == 0) {
        return;
    }
    
    if (QWidget *w = m_stack->currentWidget()) {
        m_stack->removeWidget(w);
        w->deleteLater();
    }

    if (m_stack->count() > 0) {
        m_stack->setCurrentWidget(m_stack->widget(m_stack->count() - 1));
    }

    emit countChanged(count());
    emit currentChanged(currentPage());
}

void YouTubeView::clear() {
    if (m_stack->count() == 0) {
        return;
    }
    
    for (int i = m_stack->count() - 1; i >= 0; i--) {
        if (QWidget *w = m_stack->widget(i)) {
            m_stack->removeWidget(w);
            w->deleteLater();
        }
    }

    emit countChanged(0);
    emit currentChanged(0);
}

void YouTubeView::setCurrentPage(const QModelIndex &index) {
    switch (index.row()) {
    case 0:
        listAccounts();
        break;
    case 1:
        showSearchDialog();
        break;
    case 2:
        listCategories();
        break;
    case 3:
        listUploads();
        break;
    case 4:
        listFavourites();
        break;
    case 5:
        listLikes();
        break;
    case 6:
        listWatchLater();
        break;
    case 7:
        listWatchHistory();
        break;
    case 8:
        listPlaylists();
        break;
    case 9:
        listSubscriptions();
        break;
    default:
        break;
    }
}

void YouTubeView::showSearchDialog() {
    YouTubeSearchDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        const QString query = dialog.query();

        if (query.isEmpty()) {
            return;
        }
        
        const QString order = dialog.order();
        const QString type = dialog.type();

        if (type == Resources::PLAYLIST) {
            searchPlaylists(query, order);
        }
        else if (type == Resources::USER) {
            searchUsers(query, order);
        }
        else if (type == Resources::VIDEO) {
            searchVideos(query, order);
        }
    }
}

void YouTubeView::listAccounts() {}

void YouTubeView::listCategories() {
    clear();
    QVariantMap filters;
    const QString locale = Settings::locale();
    filters["regionCode"] = locale.mid(locale.indexOf('_') + 1);
    QVariantMap params;
    params["h1"] = locale.left(locale.indexOf('_'));
    YouTubeCategoriesPage *page = new YouTubeCategoriesPage(this);
    page->setWindowTitle(tr("Categories"));
    page->list("/videoCategories", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listFavourites() {
    const QString playlistId = YouTube::relatedPlaylist("favorites");
    
    if (playlistId.isEmpty()) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any favourites"));
        return;
    }
    
    clear();
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    YouTubeVideosPage *page = new YouTubeVideosPage(this);
    page->setWindowTitle(tr("Favourites"));
    page->list("/playlistItems", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listLikes() {
    const QString playlistId = YouTube::relatedPlaylist("likes");
    
    if (playlistId.isEmpty()) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any likes"));
        return;
    }
    
    clear();
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    YouTubeVideosPage *page = new YouTubeVideosPage(this);
    page->setWindowTitle(tr("Likes"));
    page->list("/playlistItems", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listPlaylists() {
    clear();
    QVariantMap filters;
    filters["mine"] = true;
    QVariantMap params;
    filters["maxResults"] = MAX_RESULTS;
    YouTubePlaylistsPage *page = new YouTubePlaylistsPage(this);
    page->setWindowTitle(tr("Playlists"));
    page->list("/playlists", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listSubscriptions() {
    clear();
    QVariantMap filters;
    filters["mine"] = true;
    QVariantMap params;
    params["sort"] = "unread";
    params["maxResults"] = MAX_RESULTS;
    YouTubeUsersPage *page = new YouTubeUsersPage(this);
    page->setWindowTitle(tr("Subscriptions"));
    page->list("/subscriptions", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listUploads() {
    const QString playlistId = YouTube::relatedPlaylist("uploads");
    
    if (playlistId.isEmpty()) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any videos"));
        return;
    }
    
    clear();
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    YouTubeVideosPage *page = new YouTubeVideosPage(this);
    page->setWindowTitle(tr("My videos"));
    page->list("/playlistItems", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listWatchHistory() {
    const QString playlistId = YouTube::relatedPlaylist("watchHistory");
    
    if (playlistId.isEmpty()) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any watch history"));
        return;
    }
    
    clear();
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    YouTubeVideosPage *page = new YouTubeVideosPage(this);
    page->setWindowTitle(tr("Watch history"));
    page->list("/playlistItems", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::listWatchLater() {
    const QString playlistId = YouTube::relatedPlaylist("watchLater");
    
    if (playlistId.isEmpty()) {
        QMessageBox::critical(this, tr("Not available"), tr("This channel does not have any watch later playlist"));
        return;
    }
    
    clear();
    QVariantMap filters;
    filters["playlistId"] = playlistId;
    QVariantMap params;
    params["maxResults"] = MAX_RESULTS;
    YouTubeVideosPage *page = new YouTubeVideosPage(this);
    page->setWindowTitle(tr("Watch later"));
    page->list("/playlistItems", QStringList() << "snippet", filters, params);
    push(page);
}

void YouTubeView::searchPlaylists(const QString &query, const QString &order) {
    clear();
    QVariantMap params;
    params["q"] = query;
    params["order"] = order;
    params["type"] = "playlist";
    params["maxResults"] = MAX_RESULTS;
    YouTubePlaylistsPage *page = new YouTubePlaylistsPage(this);
    page->setWindowTitle(tr("Search playlists ('%1')").arg(query));
    page->list("/search", QStringList() << "snippet", QVariantMap(), params);
    push(page);
}

void YouTubeView::searchUsers(const QString &query, const QString &order) {
    clear();
    QVariantMap params;
    params["q"] = query;
    params["order"] = order;
    params["type"] = "channel";
    params["maxResults"] = MAX_RESULTS;
    YouTubeUsersPage *page = new YouTubeUsersPage(this);
    page->setWindowTitle(tr("Search channels ('%1')").arg(query));
    page->list("/search", QStringList() << "snippet", QVariantMap(), params);
    push(page);
}

void YouTubeView::searchVideos(const QString &query, const QString &order) {
    clear();
    QVariantMap params;
    params["q"] = query;
    params["order"] = order;
    params["type"] = "video";
    params["maxResults"] = MAX_RESULTS;
    YouTubeVideosPage *page = new YouTubeVideosPage(this);
    page->setWindowTitle(tr("Search videos ('%1')").arg(query));
    page->list("/search", QStringList() << "snippet", QVariantMap(), params);
    push(page);
}
