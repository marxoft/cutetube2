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

#include "vimeovideoswindow.h"
#include "clipboard.h"
#include "imagecache.h"
#include "listview.h"
#include "resources.h"
#include "settings.h"
#include "transfers.h"
#include "videodelegate.h"
#include "videolauncher.h"
#include "videoplaybackwindow.h"
#include "vimeo.h"
#include "vimeodownloaddialog.h"
#include "vimeoplaybackdialog.h"
#include "vimeoplaylistdialog.h"
#include "vimeovideowindow.h"
#include <qvimeo/urls.h>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMaemo5InformationBox>

VimeoVideosWindow::VimeoVideosWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new VimeoVideoModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, VimeoVideoModel::DateRole, VimeoVideoModel::DurationRole,
                                 VimeoVideoModel::ThumbnailUrlRole, VimeoVideoModel::TitleRole,
                                 VimeoVideoModel::UsernameRole, m_view)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_watchLaterAction(0),
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
    
    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    
    if (!Vimeo::userId().isEmpty()) {
        if (Vimeo::hasScope(QVimeo::INTERACT_SCOPE)) {
            m_favouriteAction = new QAction(this);
            m_watchLaterAction = new QAction(tr("Watch later"), this);
            m_contextMenu->addAction(m_favouriteAction);
            m_contextMenu->addAction(m_watchLaterAction);
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
            connect(m_watchLaterAction, SIGNAL(triggered()), this, SLOT(watchVideoLater()));
        }
        
        if (Vimeo::hasScope(QVimeo::EDIT_SCOPE)) {
            m_playlistAction = new QAction(tr("Add to album"), this);
            m_contextMenu->addAction(m_playlistAction);
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));
        }
    }
}

VimeoVideosWindow::~VimeoVideosWindow() {
    delete m_cache;
    m_cache = 0;
}

void VimeoVideosWindow::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void VimeoVideosWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        VimeoPlaylistDialog(video, this).exec();
    }
}

void VimeoVideosWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    const QModelIndex index = m_view->currentIndex();
    
    if (index.isValid()) {
        const QString id = index.data(VimeoVideoModel::IdRole).toString();
        const QString title = index.data(VimeoVideoModel::TitleRole).toString();
        
        VimeoDownloadDialog dialog(this);
        dialog.list(id);

        if (dialog.exec() == QDialog::Accepted) {
            Transfers::instance()->addDownloadTransfer(Resources::VIMEO, id, dialog.streamId(),
                                                       QUrl(), title, dialog.category(),
                                                       dialog.subtitlesLanguage(), dialog.customCommand(),
                                                       dialog.customCommandOverrideEnabled());
        }
    }
}

void VimeoVideosWindow::playVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::videoPlayer() == "cutetube") {
        if (VimeoVideo *video = m_model->get(index.row())) {
            VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
            window->show();
            window->addVideo(video);
        }
    }
    else {    
        VimeoPlaybackDialog dialog(this);
        dialog.list(index.data(VimeoVideoModel::IdRole).toString());

        if ((dialog.exec() == QDialog::Accepted) && (!VideoLauncher::playVideo(dialog.streamUrl()))) {
            QMessageBox::critical(this, tr("Error"), tr("Unable to play video"));
        }
    }
}

void VimeoVideosWindow::setVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
        
        if (video->isFavourite()) {
            video->unfavourite();
        }
        else {
            video->favourite();
        }
    }
}

void VimeoVideosWindow::shareVideo() {
    if (const VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        Clipboard::setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void VimeoVideosWindow::showVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (const VimeoVideo *video = m_model->get(index.row())) {
        VimeoVideoWindow *window = new VimeoVideoWindow(video, this);
        window->show();
    }
}

void VimeoVideosWindow::watchVideoLater() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
        video->watchLater();
    }
}

void VimeoVideosWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        if (m_favouriteAction) {
            m_favouriteAction->setText(m_view->currentIndex().data(VimeoVideoModel::FavouriteRole).toBool()
                                       ? tr("Unlike") : tr("Like"));
        }
        
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void VimeoVideosWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VimeoVideosWindow::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QVimeo::ResourcesRequest::Failed:
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

void VimeoVideosWindow::onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status status) {
    const VimeoVideo *video = qobject_cast<VimeoVideo*>(sender());
    
    if (!video) {
        return;
    }
    
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), video->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
               this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
}
