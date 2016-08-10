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

#include "vimeoplaylistspage.h"
#include "imagecache.h"
#include "playlistdelegate.h"
#include <QListView>
#include <QVBoxLayout>

VimeoPlaylistsPage::VimeoPlaylistsPage(QWidget *parent) :
    Page(parent),
    m_model(new VimeoPlaylistModel(this)),
    m_cache(new ImageCache),
    m_delegate(new PlaylistDelegate(m_cache, VimeoPlaylistModel::DateRole, VimeoPlaylistModel::ThumbnailUrlRole,
                                    VimeoPlaylistModel::TitleRole, VimeoPlaylistModel::UsernameRole,
                                    VimeoPlaylistModel::VideoCountRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
}

VimeoPlaylistsPage::~VimeoPlaylistsPage() {
    delete m_cache;
}

VimeoPlaylistsPage::Status VimeoPlaylistsPage::status() const {
    return m_status;
}

void VimeoPlaylistsPage::setStatus(VimeoPlaylistsPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void VimeoPlaylistsPage::cancel() {
    m_model->cancel();
}

void VimeoPlaylistsPage::reload() {
    m_model->reload();
}

void VimeoPlaylistsPage::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void VimeoPlaylistsPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VimeoPlaylistsPage::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
