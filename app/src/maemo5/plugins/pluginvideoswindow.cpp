/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "pluginvideoswindow.h"
#include "clipboard.h"
#include "imagecache.h"
#include "listview.h"
#include "plugindownloaddialog.h"
#include "pluginplaybackdialog.h"
#include "pluginvideowindow.h"
#include "settings.h"
#include "videodelegate.h"
#include "videolauncher.h"
#include "videoplaybackwindow.h"
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QMaemo5InformationBox>

PluginVideosWindow::PluginVideosWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_model(new PluginVideoModel(this)),
    m_cache(new ImageCache),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, PluginVideoModel::DateRole, PluginVideoModel::DurationRole,
                                 PluginVideoModel::ThumbnailUrlRole, PluginVideoModel::TitleRole,
                                 PluginVideoModel::UsernameRole, this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
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
    
    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
}

PluginVideosWindow::~PluginVideosWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginVideosWindow::list(const QString &service, const QString &id) {
    m_model->setService(service);
    m_model->list(id);
}

void PluginVideosWindow::search(const QString &service, const QString &query, const QString &order) {
    m_model->setService(service);
    m_model->search(query, order);
}

void PluginVideosWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    if (m_view->currentIndex().isValid()) {
        QString id = m_view->currentIndex().data(PluginVideoModel::IdRole).toString();
        QString title = m_view->currentIndex().data(PluginVideoModel::TitleRole).toString();
        QUrl streamUrl = m_view->currentIndex().data(PluginVideoModel::StreamUrlRole).toString();
        
        PluginDownloadDialog *dialog = new PluginDownloadDialog(m_model->service(), id, streamUrl, title, this);
        dialog->open();
    }
}

void PluginVideosWindow::playVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (PluginVideo *video = m_model->get(index.row())) {
            VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
            window->show();
            window->addVideo(video);
        }
    }
    else {
        QString url = index.data(PluginVideoModel::StreamUrlRole).toString();
        
        if (!url.isEmpty()) {
            VideoLauncher::playVideo(url);
        }
        else {
            QString id = index.data(PluginVideoModel::IdRole).toString();
            QString title = index.data(PluginVideoModel::TitleRole).toString();
    
            PluginPlaybackDialog *dialog = new PluginPlaybackDialog(m_model->service(), id, title, this);
            dialog->open();
        }
    }
}

void PluginVideosWindow::shareVideo() {
    if (const PluginVideo *video = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void PluginVideosWindow::showVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (const PluginVideo *video = m_model->get(index.row())) {
        PluginVideoWindow *window = new PluginVideoWindow(video, this);
        window->show();
    }
}

void PluginVideosWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        m_downloadAction->setEnabled(m_model->data(m_view->currentIndex(), PluginVideoModel::DownloadableRole).toBool());
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void PluginVideosWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginVideosWindow::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        m_label->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
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
