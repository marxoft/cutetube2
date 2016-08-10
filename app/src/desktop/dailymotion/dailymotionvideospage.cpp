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

#include "dailymotionvideospage.h"
#include "dailymotion.h"
#include "dailymotiondownloaddialog.h"
#include "imagecache.h"
#include "resources.h"
#include "transfers.h"
#include "videodelegate.h"
#include <qdailymotion/urls.h>
#include <QListView>
#include <QMenu>
#include <QVBoxLayout>

DailymotionVideosPage::DailymotionVideosPage(QWidget *parent) :
    Page(parent),
    m_model(new DailymotionVideoModel(this)),
    m_cache(new ImageCache),
    m_delegate(new VideoDelegate(m_cache, DailymotionVideoModel::DateRole, DailymotionVideoModel::DurationRole,
                                 DailymotionVideoModel::ThumbnailUrlRole, DailymotionVideoModel::TitleRole,
                                 DailymotionVideoModel::UsernameRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_playAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Play"), this)),
    m_downloadAction(new QAction(QIcon::fromTheme("go-down"), tr("&Download"), this)),
    m_favouriteAction(new QAction(QIcon::fromTheme("starred"), tr("&Favourite"), this)),
    m_playlistAction(new QAction(QIcon::fromTheme("list-add"), tr("Add to p&laylist"), this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

DailymotionVideosPage::~DailymotionVideosPage() {
    delete m_cache;
}

DailymotionVideosPage::Status DailymotionVideosPage::status() const {
    return m_status;
}

void DailymotionVideosPage::setStatus(DailymotionVideosPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void DailymotionVideosPage::cancel() {
    m_model->cancel();
}

void DailymotionVideosPage::reload() {
    m_model->reload();
}

void DailymotionVideosPage::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void DailymotionVideosPage::addVideoToPlaylist(const QModelIndex &) {

}

void DailymotionVideosPage::downloadVideo(const QModelIndex &index) {
    const QString videoId = index.data(DailymotionVideoModel::IdRole).toString();
    const QString title = index.data(DailymotionVideoModel::TitleRole).toString();
    DailymotionDownloadDialog dialog(this);
    dialog.list(videoId);

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(Resources::DAILYMOTION, videoId, dialog.streamId(),
                                                   QUrl(), title, dialog.category(), dialog.subtitlesLanguage(),
                                                   dialog.customCommand(), dialog.customCommandOverrideEnabled());
    }
}

void DailymotionVideosPage::playVideo(const QModelIndex &) {

}

void DailymotionVideosPage::setVideoFavourite(const QModelIndex &index) {
    if (DailymotionVideo *video = m_model->get(index.row())) {
        if (video->isFavourite()) {
            video->unfavourite();
        }
        else {
            video->favourite();
        }
    }
}

void DailymotionVideosPage::showVideo(const QModelIndex &) {

}

void DailymotionVideosPage::showContextMenu(const QPoint &pos) {
    if (!m_view->currentIndex().isValid()) {
        return;
    }

    QMenu menu(this);
    menu.addAction(m_playAction);
    menu.addAction(m_downloadAction);

    if (!Dailymotion::userId().isEmpty()) {
        if (Dailymotion::hasScope(QDailymotion::MANAGE_FAVORITES_SCOPE)) {            
            if (m_view->currentIndex().data(DailymotionVideoModel::FavouriteRole).toBool()) {
                m_favouriteAction->setIcon(QIcon::fromTheme("non-starred"));
                m_favouriteAction->setText(tr("Un&favourite"));
            }
            else {
                m_favouriteAction->setIcon(QIcon::fromTheme("starred"));
                m_favouriteAction->setText(tr("&Favourite"));
            }

            menu.addAction(m_favouriteAction);
        }

        if (Dailymotion::hasScope(QDailymotion::MANAGE_PLAYLISTS_SCOPE)) {
            menu.addAction(m_playlistAction);
        }
    }

    const QAction *action = menu.exec(m_view->mapToGlobal(pos));

    if (action == m_playAction) {
        playVideo(m_view->currentIndex());
    }
    else if (action == m_downloadAction) {
        downloadVideo(m_view->currentIndex());
    }
    else if (action == m_favouriteAction) {
        setVideoFavourite(m_view->currentIndex());
    }
    else if (action == m_playlistAction) {
        addVideoToPlaylist(m_view->currentIndex());
    }
}

void DailymotionVideosPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void DailymotionVideosPage::onModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
