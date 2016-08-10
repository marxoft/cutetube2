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

#include "pluginview.h"
#include "plugincategoriespage.h"
#include "pluginmanager.h"
#include "pluginnavmodel.h"
#include "pluginplaylistspage.h"
#include "pluginsearchdialog.h"
#include "pluginuserspage.h"
#include "pluginvideospage.h"
#include "resources.h"
#include <QListView>
#include <QStackedWidget>
#include <QSplitter>
#include <QVBoxLayout>

PluginView::PluginView(const QString &service, QWidget *parent) :
    PageStack(parent),
    m_model(new PluginNavModel(this)),
    m_view(new QListView(this)),
    m_stack(new QStackedWidget(this)),
    m_splitter(new QSplitter(Qt::Horizontal, this)),
    m_layout(new QVBoxLayout(this))
{
    if (ServicePluginConfig *config = PluginManager::instance()->getConfigForService(service)) {
        setWindowTitle(config->displayName());
    }
    
    m_model->setService(service);
    
    m_view->setModel(m_model);

    m_splitter->addWidget(m_view);
    m_splitter->addWidget(m_stack);
    m_splitter->setStretchFactor(1, 1);

    m_layout->addWidget(m_splitter);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(setCurrentPage(QModelIndex)));
}

int PluginView::count() const {
    return m_stack->count();
}

Page* PluginView::currentPage() const {
    return qobject_cast<Page*>(m_stack->currentWidget());
}

void PluginView::push(Page *page) {
    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit countChanged(count());
    emit currentChanged(page);
}

void PluginView::replace(Page *page) {
    if (QWidget *w = m_stack->currentWidget()) {
        m_stack->removeWidget(w);
        w->deleteLater();
    }

    page->setStack(this);
    m_stack->addWidget(page);
    m_stack->setCurrentWidget(page);
    emit currentChanged(page);
}

void PluginView::pop() {
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

void PluginView::clear() {
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

void PluginView::setCurrentPage(const QModelIndex &index) {
    const QVariantMap resource = index.data(PluginNavModel::ValueRole).toMap();
    
    if (resource.isEmpty()) {
        showSearchDialog();
    }
    else {
        const QString type = resource.value("type").toString();
        const QString id = resource.value("id").toString();
        const QString title = index.data(PluginNavModel::NameRole).toString();
        
        if (type == Resources::CATEGORY) {
            listCategories(title, id);
        }
        else if (type == Resources::PLAYLIST) {
            listPlaylists(title, id);
        }
        else if (type == Resources::USER) {
            listUsers(title, id);
        }
        else if (type == Resources::VIDEO) {
            listVideos(title, id);
        }
    }
}

void PluginView::showSearchDialog() {
    PluginSearchDialog dialog(m_model->service(), this);

    if (dialog.exec() == QDialog::Accepted) {
        const QString query = dialog.query();

        if (query.isEmpty()) {
            return;
        }
        
        const QString order = dialog.order();
        const QString type = dialog.type();

        if (type == Resources::CATEGORY) {
            searchCategories(query, order);
        }
        else if (type == Resources::PLAYLIST) {
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

void PluginView::listCategories(const QString &title, const QString &resourceId) {
    clear();
    PluginCategoriesPage *page = new PluginCategoriesPage(m_model->service(), this);
    page->setWindowTitle(title);
    page->list(resourceId);
    push(page);
}

void PluginView::listPlaylists(const QString &title, const QString &resourceId) {
    clear();
    PluginPlaylistsPage *page = new PluginPlaylistsPage(m_model->service(), this);
    page->setWindowTitle(title);
    page->list(resourceId);
    push(page);
}

void PluginView::listUsers(const QString &title, const QString &resourceId) {
    clear();
    PluginUsersPage *page = new PluginUsersPage(m_model->service(), this);
    page->setWindowTitle(title);
    page->list(resourceId);
    push(page);
}

void PluginView::listVideos(const QString &title, const QString &resourceId) {
    clear();
    PluginVideosPage *page = new PluginVideosPage(m_model->service(), this);
    page->setWindowTitle(title);
    page->list(resourceId);
    push(page);
}

void PluginView::searchCategories(const QString &query, const QString &order) {
    clear();
    PluginCategoriesPage *page = new PluginCategoriesPage(m_model->service(), this);
    page->setWindowTitle(tr("Search categories ('%1')").arg(query));
    page->search(query, order);
    push(page);
}

void PluginView::searchPlaylists(const QString &query, const QString &order) {
    clear();
    PluginPlaylistsPage *page = new PluginPlaylistsPage(m_model->service(), this);
    page->setWindowTitle(tr("Search playlists ('%1')").arg(query));
    page->search(query, order);
    push(page);
}

void PluginView::searchUsers(const QString &query, const QString &order) {
    clear();
    PluginUsersPage *page = new PluginUsersPage(m_model->service(), this);
    page->setWindowTitle(tr("Search users ('%1')").arg(query));
    page->search(query, order);
    push(page);
}

void PluginView::searchVideos(const QString &query, const QString &order) {
    clear();
    PluginVideosPage *page = new PluginVideosPage(m_model->service(), this);
    page->setWindowTitle(tr("Search videos ('%1')").arg(query));
    page->search(query, order);
    push(page);
}
