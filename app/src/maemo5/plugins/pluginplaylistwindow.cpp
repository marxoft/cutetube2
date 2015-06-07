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

#include "pluginplaylistwindow.h"
#include "clipboard.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "playlistthumbnail.h"
#include "plugindownloaddialog.h"
#include "pluginplaybackdialog.h"
#include "pluginplaylistwindow.h"
#include "pluginuser.h"
#include "pluginuserwindow.h"
#include "pluginvideomodel.h"
#include "pluginvideowindow.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "videodelegate.h"
#include "videolauncher.h"
#include "videoplaybackwindow.h"
#include <QScrollArea>
#include <QLabel>
#include <QActionGroup>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

PluginPlaylistWindow::PluginPlaylistWindow(const QString &service, const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new PluginPlaylist(this)),
    m_user(new PluginUser(this)),
    m_model(new PluginVideoModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new PlaylistThumbnail(this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, PluginVideoModel::DateRole, PluginVideoModel::DurationRole,
                                 PluginVideoModel::ThumbnailUrlRole, PluginVideoModel::TitleRole,
                                 PluginVideoModel::UsernameRole, this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_userLabel(new QLabel(this)),
    m_noVideosLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this)),
    m_viewGroup(new QActionGroup(this)),
    m_listAction(new QAction(tr("List"), this)),
    m_gridAction(new QAction(tr("Grid"), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    connect(m_playlist, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onPlaylistStatusChanged(ResourcesRequest::Status)));
            
    m_playlist->loadPlaylist(service, id);
}

PluginPlaylistWindow::PluginPlaylistWindow(const PluginPlaylist *playlist, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new PluginPlaylist(playlist, this)),
    m_user(new PluginUser(this)),
    m_model(new PluginVideoModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new PlaylistThumbnail(this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, PluginVideoModel::DateRole, PluginVideoModel::DurationRole,
                                 PluginVideoModel::ThumbnailUrlRole, PluginVideoModel::TitleRole,
                                 PluginVideoModel::UsernameRole, this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_userLabel(new QLabel(this)),
    m_noVideosLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this)),
    m_viewGroup(new QActionGroup(this)),
    m_listAction(new QAction(tr("List"), this)),
    m_gridAction(new QAction(tr("Grid"), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    loadPlaylistUi();
    getVideos();
    
    if (!playlist->userId().isEmpty()) {
        connect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(ResourcesRequest::Status)));
            
        m_user->loadUser(playlist->service(), playlist->userId());
    }
}

PluginPlaylistWindow::~PluginPlaylistWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginPlaylistWindow::loadBaseUi() {
    setWindowTitle(tr("Playlist"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 240);
    m_avatar->setFixedSize(60, 60);
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 18px").arg(palette().color(QPalette::Mid).name()));
    m_userLabel->setStyleSheet("font-size: 18px");
    m_noVideosLabel->hide();
    
    m_listAction->setCheckable(true);
    m_listAction->setChecked(true);
    m_gridAction->setCheckable(true);
    m_viewGroup->setExclusive(true);
    m_viewGroup->addAction(m_listAction);
    m_viewGroup->addAction(m_gridAction);
    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_downloadAction);
    m_contextMenu->addAction(m_shareAction);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QGridLayout *grid = new QGridLayout(scrollWidget);
    grid->addWidget(m_thumbnail, 0, 0, 1, 2, Qt::AlignLeft);
    grid->addWidget(m_titleLabel, 1, 0, 1, 2);
    grid->addWidget(m_avatar, 2, 0, 2, 1, Qt::AlignLeft);
    grid->addWidget(m_userLabel, 2, 1, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(m_dateLabel, 3, 1, Qt::AlignLeft | Qt::AlignBottom);
    grid->addWidget(m_descriptionLabel, 4, 0, 1, 2);
    grid->setRowStretch(4, 1);
    grid->setColumnStretch(1, 1);
    grid->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFixedWidth(330);
    
    m_layout = new QHBoxLayout(centralWidget());
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_view);
    m_layout->addWidget(m_noVideosLabel);
    m_layout->setStretch(1, 1);
    m_layout->setStretch(2, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_listAction);
    menuBar()->addAction(m_gridAction);
    menuBar()->addAction(m_reloadAction);
    
    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_listAction, SIGNAL(triggered()), this, SLOT(enableListMode()));
    connect(m_gridAction, SIGNAL(triggered()), this, SLOT(enableGridMode()));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playPlaylist()));
    }
    
    if (Settings::instance()->defaultViewMode() == "grid") {
        m_gridAction->trigger();
    }
}

void PluginPlaylistWindow::loadPlaylistUi() {
    m_titleLabel->setText(m_playlist->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_playlist->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_playlist->description()));
    m_thumbnail->setSource(m_playlist->largeThumbnailUrl());
    m_thumbnail->setVideoCount(m_playlist->videoCount());
}

void PluginPlaylistWindow::loadUserUi() {
    m_userLabel->setText(m_userLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_avatar->setSource(m_user->thumbnailUrl());
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showUser()));
}

void PluginPlaylistWindow::enableGridMode() {
    m_delegate->setGridMode(true);
    m_view->setFlow(QListView::LeftToRight);
    m_view->setWrapping(true);
    Settings::instance()->setDefaultViewMode("grid");
}

void PluginPlaylistWindow::enableListMode() {
    m_delegate->setGridMode(false);
    m_view->setFlow(QListView::TopToBottom);
    m_view->setWrapping(false);
    Settings::instance()->setDefaultViewMode("list");
}

void PluginPlaylistWindow::getVideos() {
    m_model->setService(m_playlist->service());
    m_model->list(m_playlist->id());
}

void PluginPlaylistWindow::playPlaylist() {
    if ((isBusy()) || (m_model->rowCount() == 0)) {
        return;
    }
    
    QList<CTVideo*> list;
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (CTVideo *video = m_model->get(i)) {
            list << video;
        }
    }
    
    if (!list.isEmpty()) {
        VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
        window->show();
        window->addVideos(list);
    }
}

void PluginPlaylistWindow::downloadVideo() {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        QString id = m_view->currentIndex().data(PluginVideoModel::IdRole).toString();
        QString title = m_view->currentIndex().data(PluginVideoModel::TitleRole).toString();
        
        PluginDownloadDialog *dialog = new PluginDownloadDialog(m_playlist->service(), id, title, this);
        dialog->open();
    }
}

void PluginPlaylistWindow::playVideo(const QModelIndex &index) {
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
    
            PluginPlaybackDialog *dialog = new PluginPlaybackDialog(m_playlist->service(), id, title, this);
            dialog->open();
        }
    }
}

void PluginPlaylistWindow::shareVideo() {
    if (const PluginVideo *video = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void PluginPlaylistWindow::showVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (const PluginVideo *video = m_model->get(index.row())) {
        PluginVideoWindow *window = new PluginVideoWindow(video, this);
        window->show();
    }
}

void PluginPlaylistWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void PluginPlaylistWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != m_playlist->service()) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_playlist->service(), resource.value("id").toString(),
                                                                this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        PluginUserWindow *window = new PluginUserWindow(m_playlist->service(), resource.value("id").toString(), this);
        window->show();
    }
    else {
        PluginVideoWindow *window = new PluginVideoWindow(m_playlist->service(), resource.value("id").toString(), this);
        window->show();
    }
}

void PluginPlaylistWindow::showUser() {
    PluginUserWindow *window = new PluginUserWindow(m_user, this);
    window->show();
}

void PluginPlaylistWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void PluginPlaylistWindow::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        m_noVideosLabel->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Ready:
        if (m_model->rowCount() > 0) {
            m_model->fetchMore();
        }
        
        break;
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
        m_noVideosLabel->show();
    }
}

void PluginPlaylistWindow::onPlaylistStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadPlaylistUi();
        getVideos();
        
        if (!m_playlist->userId().isEmpty()) {
            connect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                    SLOT(onUserStatusChanged(ResourcesRequest::Status)));
            m_user->loadUser(m_playlist->service(), m_playlist->userId());
        }
        
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(ResourcesRequest::Status)));
}

void PluginPlaylistWindow::onUserStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadUserUi();
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(ResourcesRequest::Status)));
}
