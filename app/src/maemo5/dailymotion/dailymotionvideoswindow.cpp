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

#include "dailymotionvideoswindow.h"
#include "clipboard.h"
#include "dailymotion.h"
#include "dailymotiondownloaddialog.h"
#include "dailymotionplaybackdialog.h"
#include "dailymotionplaylistdialog.h"
#include "dailymotionvideowindow.h"
#include "imagecache.h"
#include "listview.h"
#include "resources.h"
#include "settings.h"
#include "transfers.h"
#include "videodelegate.h"
#include "videolauncher.h"
#include "videoplaybackwindow.h"
#include <qdailymotion/urls.h>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMaemo5InformationBox>

DailymotionVideosWindow::DailymotionVideosWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new DailymotionVideoModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, DailymotionVideoModel::DateRole, DailymotionVideoModel::DurationRole,
                                 DailymotionVideoModel::ThumbnailUrlRole, DailymotionVideoModel::TitleRole,
                                 DailymotionVideoModel::UsernameRole, this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_playlistAction(0),
    m_label(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                              .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this))
{
    setWindowTitle(tr("Videos"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_downloadAction);  
    m_contextMenu->addAction(m_shareAction);  
    
    m_label->hide();
    
    m_layout = new QVBoxLayout(centralWidget());
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_label);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_reloadAction);
    
    connect(m_model, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    
    if (!Dailymotion::userId().isEmpty()) {
        if (Dailymotion::hasScope(QDailymotion::MANAGE_FAVORITES_SCOPE)) {
            m_favouriteAction = new QAction(this);
            m_contextMenu->addAction(m_favouriteAction);
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
        }
        
        if (Dailymotion::hasScope(QDailymotion::MANAGE_PLAYLISTS_SCOPE)) {
            m_playlistAction = new QAction(tr("Add to playlist"), this);
            m_contextMenu->addAction(m_playlistAction);
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));
        }
    }
}

DailymotionVideosWindow::~DailymotionVideosWindow() {
    delete m_cache;
    m_cache = 0;
}

void DailymotionVideosWindow::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void DailymotionVideosWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (DailymotionVideo *video = m_model->get(m_view->currentIndex().row())) {
        DailymotionPlaylistDialog(video, this).exec();
    }
}

void DailymotionVideosWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    const QModelIndex index = m_view->currentIndex();
    
    if (index.isValid()) {
        const QString id = index.data(DailymotionVideoModel::IdRole).toString();
        const QString title = index.data(DailymotionVideoModel::TitleRole).toString();
        
        DailymotionDownloadDialog dialog(this);
        dialog.list(id);

        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(Resources::DAILYMOTION, id, dialog.streamId(),
                                                       QUrl(), title, dialog.category(),
                                                       dialog.subtitlesLanguage(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
}

void DailymotionVideosWindow::playVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::videoPlayer() == "cutetube") {
        if (DailymotionVideo *video = m_model->get(index.row())) {
            VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
            window->show();
            window->addVideo(video);
        }
    }
    else {    
        DailymotionPlaybackDialog dialog(this);
        dialog.list(index.data(DailymotionVideoModel::IdRole).toString());

        if ((dialog.exec() == QDialog::Accepted) && (!VideoLauncher::playVideo(dialog.streamUrl()))) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to play video"));
        }
    }
}

void DailymotionVideosWindow::setVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (DailymotionVideo *video = m_model->get(m_view->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status)));
        
        if (video->isFavourite()) {
            video->unfavourite();
        }
        else {
            video->favourite();
        }
    }
}

void DailymotionVideosWindow::shareVideo() {
    if (const DailymotionVideo *video = m_model->get(m_view->currentIndex().row())) {
        Clipboard::setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void DailymotionVideosWindow::showVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (const DailymotionVideo *video = m_model->get(index.row())) {
        DailymotionVideoWindow *window = new DailymotionVideoWindow(video, this);
        window->show();
    }
}

void DailymotionVideosWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        if (m_favouriteAction) {
            m_favouriteAction->setText(m_view->currentIndex().data(DailymotionVideoModel::FavouriteRole).toBool()
                                       ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void DailymotionVideosWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void DailymotionVideosWindow::onModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_model->rowCount() == 0) {
        m_view->hide();
        m_label->show();
    }
}

void DailymotionVideosWindow::onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    const DailymotionVideo *video = qobject_cast<DailymotionVideo*>(sender());
    
    if (!video) {
        return;
    }
    
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), video->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(video, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
               this, SLOT(onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status)));
}
