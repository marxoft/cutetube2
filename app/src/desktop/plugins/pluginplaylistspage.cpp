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

#include "pluginplaylistspage.h"
#include "imagecache.h"
#include "playlistdelegate.h"
#include <QListView>
#include <QVBoxLayout>

PluginPlaylistsPage::PluginPlaylistsPage(const QString &service, QWidget *parent) :
    Page(parent),
    m_model(new PluginPlaylistModel(this)),
    m_cache(new ImageCache),
    m_delegate(new PlaylistDelegate(m_cache, PluginPlaylistModel::DateRole, PluginPlaylistModel::ThumbnailUrlRole,
                                    PluginPlaylistModel::TitleRole, PluginPlaylistModel::UsernameRole,
                                    PluginPlaylistModel::VideoCountRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_model->setService(service);

    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
}

PluginPlaylistsPage::~PluginPlaylistsPage() {
    delete m_cache;
}

PluginPlaylistsPage::Status PluginPlaylistsPage::status() const {
    return m_status;
}

void PluginPlaylistsPage::setStatus(PluginPlaylistsPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void PluginPlaylistsPage::cancel() {
    m_model->cancel();
}

void PluginPlaylistsPage::reload() {
    m_model->reload();
}

void PluginPlaylistsPage::list(const QString &resourceId) {
    m_model->list(resourceId);
}

void PluginPlaylistsPage::search(const QString &query, const QString &order) {
    m_model->search(query, order);
}

void PluginPlaylistsPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginPlaylistsPage::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
