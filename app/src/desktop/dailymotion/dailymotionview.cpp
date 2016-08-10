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
#include "dailymotioncategoriespage.h"
#include "dailymotionnavmodel.h"
#include "dailymotionplaylistspage.h"
#include "dailymotionsearchdialog.h"
#include "dailymotionuserspage.h"
#include "dailymotionvideospage.h"
#include "definitions.h"
#include "resources.h"
#include <QListView>
#include <QStackedWidget>
#include <QSplitter>
#include <QVBoxLayout>

DailymotionView::DailymotionView(QWidget *parent) :
    PageStack(parent),
    m_model(new DailymotionNavModel(this)),
    m_view(new QListView(this)),
    m_stack(new QStackedWidget(this)),
    m_splitter(new QSplitter(Qt::Horizontal, this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle("Dailymotion");
    
    m_view->setModel(m_model);

    m_splitter->addWidget(m_view);
    m_splitter->addWidget(m_stack);
    m_splitter->setStretchFactor(1, 1);

    m_layout->addWidget(m_splitter);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentPage(QModelIndex)));
}

int DailymotionView::count() const {
    return m_stack->count();
}

Page* DailymotionView::currentPage() const {
    return qobject_cast<Page*>(m_stack->currentWidget());
}

void DailymotionView::push(Page *page) {
    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit countChanged(count());
    emit currentChanged(page);
}

void DailymotionView::replace(Page *page) {
    if (QWidget *w = m_stack->currentWidget()) {
        m_stack->removeWidget(w);
        w->deleteLater();
    }

    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit currentChanged(page);
}

void DailymotionView::pop() {
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

void DailymotionView::clear() {
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

void DailymotionView::setCurrentPage(const QModelIndex &index) {
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
        listLatestVideos();
        break;
    case 5:
        listFavourites();
        break;
    case 6:
        listPlaylists();
        break;
    case 7:
        listSubscriptions();
        break;
    default:
        break;
    }
}

void DailymotionView::showSearchDialog() {
    DailymotionSearchDialog dialog(this);

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

void DailymotionView::listAccounts() {}

void DailymotionView::listCategories() {
    clear();
    QVariantMap filters;
    filters["limit"] = 50;
    DailymotionCategoriesPage *page = new DailymotionCategoriesPage(this);
    page->setWindowTitle(tr("Categories"));
    page->list("/channels", filters);
    push(page);
}

void DailymotionView::listFavourites() {
    clear();
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    DailymotionVideosPage *page = new DailymotionVideosPage(this);
    page->setWindowTitle(tr("Favourites"));
    page->list("/me/favorites", filters);
    push(page);
}

void DailymotionView::listLatestVideos() {
    clear();
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    DailymotionVideosPage *page = new DailymotionVideosPage(this);
    page->setWindowTitle(tr("Latest videos"));
    page->list("/me/subscriptions", filters);
    push(page);
}

void DailymotionView::listPlaylists() {
    clear();
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    DailymotionPlaylistsPage *page = new DailymotionPlaylistsPage(this);
    page->setWindowTitle(tr("Playlists"));
    page->list("/me/playlists", filters);
    push(page);
}

void DailymotionView::listSubscriptions() {
    clear();
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    DailymotionUsersPage *page = new DailymotionUsersPage(this);
    page->setWindowTitle(tr("Subscriptions"));
    page->list("/me/following", filters);
    push(page);
}

void DailymotionView::listUploads() {
    clear();
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    DailymotionVideosPage *page = new DailymotionVideosPage(this);
    page->setWindowTitle(tr("My videos"));
    page->list("/me/videos", filters);
    push(page);
}

void DailymotionView::searchPlaylists(const QString &query, const QString &order) {
    clear();
    QVariantMap filters;
    filters["search"] = query;
    filters["sort"] = order;
    filters["limit"] = MAX_RESULTS;
    DailymotionPlaylistsPage *page = new DailymotionPlaylistsPage(this);
    page->setWindowTitle(tr("Search playlists ('%1')").arg(query));
    page->list("/playlists", filters);
    push(page);
}

void DailymotionView::searchUsers(const QString &query, const QString &order) {
    clear();
    QVariantMap filters;
    filters["search"] = query;
    filters["sort"] = order;
    filters["limit"] = MAX_RESULTS;
    DailymotionUsersPage *page = new DailymotionUsersPage(this);
    page->setWindowTitle(tr("Search users ('%1')").arg(query));
    page->list("/users", filters);
    push(page);
}

void DailymotionView::searchVideos(const QString &query, const QString &order) {
    clear();
    QVariantMap filters;
    filters["search"] = query;
    filters["sort"] = order;
    filters["limit"] = MAX_RESULTS;
    DailymotionVideosPage *page = new DailymotionVideosPage(this);
    page->setWindowTitle(tr("Search videos ('%1')").arg(query));
    page->list("/videos", filters);
    push(page);
}
