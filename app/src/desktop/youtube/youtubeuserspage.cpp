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

#include "youtubeuserspage.h"
#include "imagecache.h"
#include "pagestack.h"
#include "userdelegate.h"
#include "youtubeuserpage.h"
#include <QListView>
#include <QMessageBox>
#include <QVBoxLayout>

YouTubeUsersPage::YouTubeUsersPage(QWidget *parent) :
    Page(parent),
    m_model(new YouTubeUserModel(this)),
    m_cache(new ImageCache),
    m_delegate(new UserDelegate(m_cache, -1, YouTubeUserModel::ThumbnailUrlRole, YouTubeUserModel::UsernameRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showUser(QModelIndex)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));
}

YouTubeUsersPage::~YouTubeUsersPage() {
    delete m_cache;
}

YouTubeUsersPage::Status YouTubeUsersPage::status() const {
    return m_status;
}

void YouTubeUsersPage::setStatus(YouTubeUsersPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString YouTubeUsersPage::statusText() const {
    return m_statusText;
}

void YouTubeUsersPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void YouTubeUsersPage::cancel() {
    m_model->cancel();
}

void YouTubeUsersPage::reload() {
    m_model->reload();
}

void YouTubeUsersPage::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                            const QVariantMap &params) {
    m_model->list(resourcePath, part, filters, params);
}

void YouTubeUsersPage::showUser(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        if (const YouTubeUser *user = m_model->get(index.row())) {
            s->push(new YouTubeUserPage(user, this));
        }
    }
}

void YouTubeUsersPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void YouTubeUsersPage::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading channels"));
        return;
    case QYouTube::ResourcesRequest::Ready:
        setStatusText(m_model->rowCount() > 0 ? tr("Ready") : tr("No channels found"));
        break;
    case QYouTube::ResourcesRequest::Canceled:
        setStatusText(tr("Canceled"));
        break;
    case QYouTube::ResourcesRequest::Failed:
        setStatusText(m_model->errorString());
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:        
        break;
    }
    
    setStatus(Ready);
}
