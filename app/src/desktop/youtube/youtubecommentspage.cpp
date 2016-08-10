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

#include "youtubecommentspage.h"
#include "commentdelegate.h"
#include "imagecache.h"
#include "pagestack.h"
#include "youtubeuserpage.h"
#include <QListView>
#include <QMessageBox>
#include <QVBoxLayout>

YouTubeCommentsPage::YouTubeCommentsPage(QWidget *parent) :
    Page(parent),
    m_model(new YouTubeCommentModel(this)),
    m_cache(new ImageCache),
    m_delegate(new CommentDelegate(m_cache, YouTubeCommentModel::BodyRole, YouTubeCommentModel::DateRole,
                                   YouTubeCommentModel::ThumbnailUrlRole, YouTubeCommentModel::UsernameRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setUniformItemSizes(false);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));
}

YouTubeCommentsPage::~YouTubeCommentsPage() {
    delete m_cache;
}

YouTubeCommentsPage::Status YouTubeCommentsPage::status() const {
    return m_status;
}

void YouTubeCommentsPage::setStatus(YouTubeCommentsPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString YouTubeCommentsPage::statusText() const {
    return m_statusText;
}

void YouTubeCommentsPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void YouTubeCommentsPage::cancel() {
    m_model->cancel();
}

void YouTubeCommentsPage::reload() {
    m_model->reload();
}

void YouTubeCommentsPage::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                            const QVariantMap &params) {
    m_model->list(resourcePath, part, filters, params);
}

void YouTubeCommentsPage::showUser(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        s->push(new YouTubeUserPage(index.data(YouTubeCommentModel::UserIdRole).toString(), this));
    }
}

void YouTubeCommentsPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void YouTubeCommentsPage::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading comments"));
        return;
    case QYouTube::ResourcesRequest::Ready:
        setStatusText(m_model->rowCount() > 0 ? tr("Ready") : tr("No comments found"));
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
