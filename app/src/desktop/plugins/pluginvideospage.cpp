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

#include "pluginvideospage.h"
#include "imagecache.h"
#include "plugindownloaddialog.h"
#include "resources.h"
#include "transfers.h"
#include "videodelegate.h"
#include "videoplayerwindow.h"
#include <QListView>
#include <QMenu>
#include <QVBoxLayout>

PluginVideosPage::PluginVideosPage(const QString &service, QWidget *parent) :
    Page(parent),
    m_model(new PluginVideoModel(this)),
    m_cache(new ImageCache),
    m_delegate(new VideoDelegate(m_cache, PluginVideoModel::DateRole, PluginVideoModel::DurationRole,
                                 PluginVideoModel::ThumbnailUrlRole, PluginVideoModel::TitleRole,
                                 PluginVideoModel::UsernameRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_model->setService(service);

    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

PluginVideosPage::~PluginVideosPage() {
    delete m_cache;
}

PluginVideosPage::Status PluginVideosPage::status() const {
    return m_status;
}

void PluginVideosPage::setStatus(PluginVideosPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void PluginVideosPage::cancel() {
    m_model->cancel();
}

void PluginVideosPage::reload() {
    m_model->reload();
}

void PluginVideosPage::list(const QString &resourceId) {
    m_model->list(resourceId);
}

void PluginVideosPage::search(const QString &query, const QString &order) {
    m_model->search(query, order);
}

void PluginVideosPage::downloadVideo(const QModelIndex &index) {
    const QString videoId = index.data(PluginVideoModel::IdRole).toString();
    const QString title = index.data(PluginVideoModel::TitleRole).toString();
    const QUrl streamUrl = index.data(PluginVideoModel::StreamUrlRole).toString();
    const bool subtitles = index.data(PluginVideoModel::SubtitlesRole).toBool();
            
    PluginDownloadDialog dialog(m_model->service(), this);
    dialog.list(videoId, streamUrl.isEmpty(), subtitles);

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(m_model->service(), videoId, dialog.streamId(),
                                                   streamUrl, title, dialog.category(),
                                                   dialog.subtitlesLanguage(), dialog.customCommand(),
                                                   dialog.customCommandOverrideEnabled());
    }
}

void PluginVideosPage::playVideo(const QModelIndex &index) {
    if (PluginVideo *video = m_model->get(index.row())) {
        VideoPlayerWindow::instance()->show();
        VideoPlayerWindow::instance()->activateWindow();
        VideoPlayerWindow::instance()->play(video);
    }
}

void PluginVideosPage::showVideo(const QModelIndex &) {

}

void PluginVideosPage::showContextMenu(const QPoint &pos) {
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

void PluginVideosPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginVideosPage::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
