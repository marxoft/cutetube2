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

#include "vimeoview.h"
#include "definitions.h"
#include "resources.h"
#include "vimeocategoriespage.h"
#include "vimeonavmodel.h"
#include "vimeoplaylistspage.h"
#include "vimeosearchdialog.h"
#include "vimeouserspage.h"
#include "vimeovideospage.h"
#include <QListView>
#include <QStackedWidget>
#include <QSplitter>
#include <QVBoxLayout>

VimeoView::VimeoView(QWidget *parent) :
    PageStack(parent),
    m_model(new VimeoNavModel(this)),
    m_view(new QListView(this)),
    m_stack(new QStackedWidget(this)),
    m_splitter(new QSplitter(Qt::Horizontal, this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle("Vimeo");
    
    m_view->setModel(m_model);

    m_splitter->addWidget(m_view);
    m_splitter->addWidget(m_stack);
    m_splitter->setStretchFactor(1, 1);

    m_layout->addWidget(m_splitter);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentPage(QModelIndex)));
}

int VimeoView::count() const {
    return m_stack->count();
}

Page* VimeoView::currentPage() const {
    return qobject_cast<Page*>(m_stack->currentWidget());
}

void VimeoView::push(Page *page) {
    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit countChanged(count());
    emit currentChanged(page);
}

void VimeoView::replace(Page *page) {
    if (QWidget *w = m_stack->currentWidget()) {
        m_stack->removeWidget(w);
        w->deleteLater();
    }

    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit currentChanged(page);
}

void VimeoView::pop() {
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

void VimeoView::clear() {
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

void VimeoView::setCurrentPage(const QModelIndex &index) {
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

void VimeoView::showSearchDialog() {
    VimeoSearchDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted) {
        const QString query = dialog.query();

        if (query.isEmpty()) {
            return;
        }
        
        const QString order = dialog.order();
        const QString type = dialog.type();

        if (type == Resources::USER) {
            searchUsers(query, order);
        }
        else if (type == Resources::VIDEO) {
            searchVideos(query, order);
        }
    }
}

void VimeoView::listAccounts() {}

void VimeoView::listCategories() {
    clear();
    QVariantMap filters;
    filters["per_page"] = 50;
    VimeoCategoriesPage *page = new VimeoCategoriesPage(this);
    page->setWindowTitle(tr("Categories"));
    page->list("/categories", filters);
    push(page);
}

void VimeoView::listFavourites() {
    clear();
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    VimeoVideosPage *page = new VimeoVideosPage(this);
    page->setWindowTitle(tr("Likes"));
    page->list("/me/likes", filters);
    push(page);
}

void VimeoView::listLatestVideos() {
    clear();
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    VimeoVideosPage *page = new VimeoVideosPage(this);
    page->setWindowTitle(tr("Latest videos"));
    page->list("/me/feed", filters);
    push(page);
}

void VimeoView::listPlaylists() {
    clear();
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    VimeoPlaylistsPage *page = new VimeoPlaylistsPage(this);
    page->setWindowTitle(tr("Albums"));
    page->list("/me/albums", filters);
    push(page);
}

void VimeoView::listSubscriptions() {
    clear();
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    VimeoUsersPage *page = new VimeoUsersPage(this);
    page->setWindowTitle(tr("Subscriptions"));
    page->list("/me/following", filters);
    push(page);
}

void VimeoView::listUploads() {
    clear();
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    VimeoVideosPage *page = new VimeoVideosPage(this);
    page->setWindowTitle(tr("My videos"));
    page->list("/me/videos", filters);
    push(page);
}

void VimeoView::listWatchLater() {
    clear();
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    VimeoVideosPage *page = new VimeoVideosPage(this);
    page->setWindowTitle(tr("Watch later"));
    page->list("/me/watchlater", filters);
    push(page);
}

void VimeoView::searchUsers(const QString &query, const QString &order) {
    clear();
    QVariantMap filters;
    filters["query"] = query;
    filters["sort"] = order;
    filters["per_page"] = MAX_RESULTS;
    VimeoUsersPage *page = new VimeoUsersPage(this);
    page->setWindowTitle(tr("Search users ('%1')").arg(query));
    page->list("/users", filters);
    push(page);
}

void VimeoView::searchVideos(const QString &query, const QString &order) {
    clear();
    QVariantMap filters;
    filters["query"] = query;
    filters["sort"] = order;
    filters["per_page"] = MAX_RESULTS;
    VimeoVideosPage *page = new VimeoVideosPage(this);
    page->setWindowTitle(tr("Search videos ('%1')").arg(query));
    page->list("/videos", filters);
    push(page);
}
