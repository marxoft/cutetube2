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

#include "youtubevideospage.h"
#include "imagecache.h"
#include "pagestack.h"
#include "resources.h"
#include "transfers.h"
#include "videodelegate.h"
#include "videoplayerwindow.h"
#include "youtubedownloaddialog.h"
#include "youtubevideopage.h"
#include <QListView>
#include <QMenu>
#include <QMessageBox>
#include <QVBoxLayout>

YouTubeVideosPage::YouTubeVideosPage(QWidget *parent) :
    Page(parent),
    m_model(new YouTubeVideoModel(this)),
    m_cache(new ImageCache),
    m_delegate(new VideoDelegate(m_cache, YouTubeVideoModel::DateRole, YouTubeVideoModel::DurationRole,
                                 YouTubeVideoModel::ThumbnailUrlRole, YouTubeVideoModel::TitleRole,
                                 YouTubeVideoModel::UsernameRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

YouTubeVideosPage::~YouTubeVideosPage() {
    delete m_cache;
}

YouTubeVideosPage::Status YouTubeVideosPage::status() const {
    return m_status;
}

void YouTubeVideosPage::setStatus(YouTubeVideosPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString YouTubeVideosPage::statusText() const {
    return m_statusText;
}

void YouTubeVideosPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void YouTubeVideosPage::cancel() {
    m_model->cancel();
}

void YouTubeVideosPage::reload() {
    m_model->reload();
}

void YouTubeVideosPage::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                             const QVariantMap &params) {
    m_model->list(resourcePath, part, filters, params);
}

void YouTubeVideosPage::downloadVideo(const QModelIndex &index) {
    const QString videoId = index.data(YouTubeVideoModel::IdRole).toString();
    const QString title = index.data(YouTubeVideoModel::TitleRole).toString();
    YouTubeDownloadDialog dialog(this);
    dialog.list(videoId);

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(Resources::YOUTUBE, videoId, dialog.streamId(),
                                                   QUrl(), title, dialog.category(), dialog.subtitlesLanguage(),
                                                   dialog.customCommand(), dialog.customCommandOverrideEnabled());
    }
}

void YouTubeVideosPage::playVideo(const QModelIndex &index) {
    if (YouTubeVideo *video = m_model->get(index.row())) {
        VideoPlayerWindow::instance()->show();
        VideoPlayerWindow::instance()->activateWindow();
        VideoPlayerWindow::instance()->play(video);
    }
}

void YouTubeVideosPage::showVideo(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        if (const YouTubeVideo *video = m_model->get(index.row())) {
            s->push(new YouTubeVideoPage(video, this));
        }
    }
}

void YouTubeVideosPage::showContextMenu(const QPoint &pos) {
    if (!m_view->currentIndex().isValid()) {
        return;
    }

    QMenu menu(this);
    QAction *downloadAction = menu.addAction(tr("Download"));

    QAction *action = menu.exec(m_view->mapToGlobal(pos));

    if (action == downloadAction) {
        downloadVideo(m_view->currentIndex());
    }
}

void YouTubeVideosPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void YouTubeVideosPage::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading videos"));
        return;
    case QYouTube::ResourcesRequest::Ready:
        setStatusText(m_model->rowCount() > 0 ? tr("Ready") : tr("No videos found"));
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
