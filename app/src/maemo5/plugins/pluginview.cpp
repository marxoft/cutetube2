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

#include "pluginview.h"
#include "listview.h"
#include "navdelegate.h"
#include "plugincategorieswindow.h"
#include "pluginnavmodel.h"
#include "pluginplaylistswindow.h"
#include "pluginplaylistwindow.h"
#include "pluginsearchdialog.h"
#include "pluginuserswindow.h"
#include "pluginuserwindow.h"
#include "pluginvideoswindow.h"
#include "pluginvideowindow.h"
#include "resources.h"
#include "stackedwindow.h"
#include <QVBoxLayout>
#include <QMaemo5InformationBox>

PluginView::PluginView(const QString &service, QWidget *parent) :
    QWidget(parent),
    m_model(new PluginNavModel(this)),
    m_view(new ListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_model->setService(service);
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(onItemActivated(QModelIndex)));
}

void PluginView::search(const QString &query, const QString &type, const QString &order) {
    if (type == Resources::PLAYLIST) {
        PluginPlaylistsWindow *window = new PluginPlaylistsWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->search(m_model->service(), query, order);
        window->show();
    }
    else if (type == Resources::USER) {
        PluginUsersWindow *window = new PluginUsersWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->search(m_model->service(), query, order);
        window->show();
    }
    else {
        PluginVideosWindow *window = new PluginVideosWindow(StackedWindow::currentWindow());
        window->setWindowTitle(QString("%1 ('%2')").arg(tr("Search")).arg(query));
        window->search(m_model->service(), query, order);
        window->show();
    }
}

void PluginView::showResource(const QString &type, const QString &id) {
    if (type == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_model->service(), id, StackedWindow::currentWindow());
        window->show();
    }
    else if (type == Resources::USER) {
        PluginUserWindow *window = new PluginUserWindow(m_model->service(), id, StackedWindow::currentWindow());
        window->show();
    }
    else {
        PluginVideoWindow *window = new PluginVideoWindow(m_model->service(), id, StackedWindow::currentWindow());
        window->show();
    }
}

void PluginView::showCategories(const QString &name, const QString &id) {
    PluginCategoriesWindow *window = new PluginCategoriesWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::showPlaylists(const QString &name, const QString &id) {
    PluginPlaylistsWindow *window = new PluginPlaylistsWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::showSearchDialog() {
    PluginSearchDialog *dialog = new PluginSearchDialog(m_model->service(), StackedWindow::currentWindow());
    dialog->open();
}

void PluginView::showUsers(const QString &name, const QString &id) {
    PluginUsersWindow *window = new PluginUsersWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::showVideos(const QString &name, const QString &id) {
    PluginVideosWindow *window = new PluginVideosWindow(StackedWindow::currentWindow());
    window->setWindowTitle(name);
    window->list(m_model->service(), id);
    window->show();
}

void PluginView::onItemActivated(const QModelIndex &index) {
    QVariantMap type = index.data(PluginNavModel::ValueRole).toMap();
    
    if (type.isEmpty()) {
        showSearchDialog();
    }
    else if (type.value("type") == Resources::CATEGORY) {
        showCategories(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
    else if (type.value("type") == Resources::PLAYLIST) {
        showPlaylists(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
    else if (type.value("type") == Resources::USER) {
        showUsers(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
    else {
        showVideos(index.data(PluginNavModel::NameRole).toString(), type.value("id").toString());
    }
}
