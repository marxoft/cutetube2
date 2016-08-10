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

#include "vimeouserspage.h"
#include "imagecache.h"
#include "userdelegate.h"
#include <QListView>
#include <QVBoxLayout>

VimeoUsersPage::VimeoUsersPage(QWidget *parent) :
    Page(parent),
    m_model(new VimeoUserModel(this)),
    m_cache(new ImageCache),
    m_delegate(new UserDelegate(m_cache, -1, VimeoUserModel::ThumbnailUrlRole, VimeoUserModel::UsernameRole, this)),
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

VimeoUsersPage::~VimeoUsersPage() {
    delete m_cache;
}

VimeoUsersPage::Status VimeoUsersPage::status() const {
    return m_status;
}

void VimeoUsersPage::setStatus(VimeoUsersPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void VimeoUsersPage::cancel() {
    m_model->cancel();
}

void VimeoUsersPage::reload() {
    m_model->reload();
}

void VimeoUsersPage::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void VimeoUsersPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VimeoUsersPage::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
