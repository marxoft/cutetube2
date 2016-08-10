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

#include "pluginuserspage.h"
#include "imagecache.h"
#include "userdelegate.h"
#include <QListView>
#include <QVBoxLayout>

PluginUsersPage::PluginUsersPage(const QString &service, QWidget *parent) :
    Page(parent),
    m_model(new PluginUserModel(this)),
    m_cache(new ImageCache),
    m_delegate(new UserDelegate(m_cache, -1, PluginUserModel::ThumbnailUrlRole,
                                PluginUserModel::UsernameRole, this)),
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

PluginUsersPage::~PluginUsersPage() {
    delete m_cache;
}

PluginUsersPage::Status PluginUsersPage::status() const {
    return m_status;
}

void PluginUsersPage::setStatus(PluginUsersPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void PluginUsersPage::cancel() {
    m_model->cancel();
}

void PluginUsersPage::reload() {
    m_model->reload();
}

void PluginUsersPage::list(const QString &resourceId) {
    m_model->list(resourceId);
}

void PluginUsersPage::search(const QString &query, const QString &order) {
    m_model->search(query, order);
}

void PluginUsersPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginUsersPage::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
