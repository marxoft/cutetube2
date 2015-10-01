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

#include "vimeoplaylistwindow.h"
#include "clipboard.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "playlistthumbnail.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "videodelegate.h"
#include "videoplaybackwindow.h"
#include "vimeo.h"
#include "vimeodownloaddialog.h"
#include "vimeoplaybackdialog.h"
#include "vimeoplaylistdialog.h"
#include "vimeoplaylistwindow.h"
#include "vimeouser.h"
#include "vimeouserwindow.h"
#include "vimeovideomodel.h"
#include "vimeovideowindow.h"
#include <qvimeo/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

VimeoPlaylistWindow::VimeoPlaylistWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new VimeoPlaylist(this)),
    m_user(new VimeoUser(this)),
    m_model(new VimeoVideoModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new PlaylistThumbnail(this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, VimeoVideoModel::DateRole, VimeoVideoModel::DurationRole,
                                 VimeoVideoModel::ThumbnailUrlRole, VimeoVideoModel::TitleRole,
                                 VimeoVideoModel::UsernameRole, m_view)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_userLabel(new QLabel(this)),
    m_noVideosLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_watchLaterAction(0),
    m_playlistAction(0),
    m_removeAction(0)
{
    loadBaseUi();
    connect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status)));
            
    m_playlist->loadPlaylist(id);
}

VimeoPlaylistWindow::VimeoPlaylistWindow(const VimeoPlaylist *playlist, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new VimeoPlaylist(playlist, this)),
    m_user(new VimeoUser(this)),
    m_model(new VimeoVideoModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new PlaylistThumbnail(this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_delegate(new VideoDelegate(m_cache, VimeoVideoModel::DateRole, VimeoVideoModel::DurationRole,
                                 VimeoVideoModel::ThumbnailUrlRole, VimeoVideoModel::TitleRole,
                                 VimeoVideoModel::UsernameRole, m_view)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_userLabel(new QLabel(this)),
    m_noVideosLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_contextMenu(new QMenu(this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_watchLaterAction(0),
    m_playlistAction(0),
    m_removeAction(0)
{
    loadBaseUi();
    loadPlaylistUi();
    getVideos();
    connect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onPlaylistUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
    
    m_user->loadUser(playlist->userId());
}

VimeoPlaylistWindow::~VimeoPlaylistWindow() {
    delete m_cache;
    m_cache = 0;
}

void VimeoPlaylistWindow::loadBaseUi() {
    setWindowTitle(tr("Album"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 240);
    m_avatar->setFixedSize(60, 60);
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 13pt").arg(palette().color(QPalette::Mid).name()));
    m_userLabel->setStyleSheet("font-size: 13pt");
    m_noVideosLabel->hide();

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
    
    menuBar()->addAction(m_reloadAction);
    
    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showUser()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    
    if (!Vimeo::instance()->userId().isEmpty()) {
        if (Vimeo::instance()->hasScope(QVimeo::INTERACT_SCOPE)) {
            m_favouriteAction = new QAction(this);
            m_watchLaterAction = new QAction(tr("Watch later"), this);
            m_contextMenu->addAction(m_favouriteAction);
            m_contextMenu->addAction(m_watchLaterAction);
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
            connect(m_watchLaterAction, SIGNAL(triggered()), this, SLOT(watchVideoLater()));
        }
        
        if (Vimeo::instance()->hasScope(QVimeo::EDIT_SCOPE)) {
            m_playlistAction = new QAction(tr("Add to album"), this);
            m_contextMenu->addAction(m_playlistAction);
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));            
        }
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playPlaylist()));
    }
}

void VimeoPlaylistWindow::loadPlaylistUi() {
    m_titleLabel->setText(m_playlist->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_playlist->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_playlist->description()));
    m_thumbnail->setSource(m_playlist->largeThumbnailUrl());
    m_thumbnail->setVideoCount(m_playlist->videoCount());
    
    if ((!m_removeAction) && (m_playlist->userId() == Vimeo::instance()->userId())
        && (Vimeo::instance()->hasScope(QVimeo::EDIT_SCOPE))) {
        m_removeAction = new QAction(tr("Remove"), this);
        m_contextMenu->addAction(m_removeAction);
        connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeVideo()));
    }
}

void VimeoPlaylistWindow::loadUserUi() {
    m_userLabel->setText(m_userLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_avatar->setSource(m_user->thumbnailUrl());
}

void VimeoPlaylistWindow::getVideos() {
    QVariantMap filters;
    filters["per_page"] = 50;
    
    m_model->list(QString("/users/%1/albums/%2/videos").arg(m_playlist->userId()).arg(m_playlist->id()), filters);
}

void VimeoPlaylistWindow::playPlaylist() {
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

void VimeoPlaylistWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        VimeoPlaylistDialog *dialog = new VimeoPlaylistDialog(video, this);
        dialog->open();
    }
}

void VimeoPlaylistWindow::downloadVideo() {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        QString id = m_view->currentIndex().data(VimeoVideoModel::IdRole).toString();
        QString title = m_view->currentIndex().data(VimeoVideoModel::TitleRole).toString();
        
        VimeoDownloadDialog *dialog = new VimeoDownloadDialog(id, title, this);
        dialog->open();
    }
}

void VimeoPlaylistWindow::playVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (VimeoVideo *video = m_model->get(index.row())) {
            VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
            window->show();
            window->addVideo(video);
        }
    }
    else {
        QString id = index.data(VimeoVideoModel::IdRole).toString();
        QString title = index.data(VimeoVideoModel::TitleRole).toString();
    
        VimeoPlaybackDialog *dialog = new VimeoPlaybackDialog(id, title, this);
        dialog->open();
    }
}

void VimeoPlaylistWindow::removeVideo() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        m_playlist->removeVideo(video);
    }
}

void VimeoPlaylistWindow::setVideoFavourite() {
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

void VimeoPlaylistWindow::shareVideo() {
    if (const VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void VimeoPlaylistWindow::showVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (const VimeoVideo *video = m_model->get(index.row())) {
        VimeoVideoWindow *window = new VimeoVideoWindow(video, this);
        window->show();
    }
}

void VimeoPlaylistWindow::watchVideoLater() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_model->get(m_view->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
        video->watchLater();
    }
}

void VimeoPlaylistWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        if (m_favouriteAction) {
            m_favouriteAction->setText(m_view->currentIndex().data(VimeoVideoModel::FavouriteRole).toBool()
                                       ? tr("Unlike") : tr("Like"));
        }
        
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void VimeoPlaylistWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::VIMEO) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        VimeoPlaylistWindow *window = new VimeoPlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        VimeoUserWindow *window = new VimeoUserWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        VimeoVideoWindow *window = new VimeoVideoWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void VimeoPlaylistWindow::showUser() {
    VimeoUserWindow *window = new VimeoUserWindow(m_user, this);
    window->show();
}

void VimeoPlaylistWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VimeoPlaylistWindow::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        m_noVideosLabel->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QVimeo::ResourcesRequest::Ready:
        if (m_model->rowCount() > 0) {
            m_model->fetchMore();
        }
        
        break;
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
        m_noVideosLabel->show();
    }
}

void VimeoPlaylistWindow::onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        loadPlaylistUi();
        getVideos();
        connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
        m_user->loadUser(m_playlist->userId());
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
               this, SLOT(onPlaylistStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_playlist, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onPlaylistUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
}

void VimeoPlaylistWindow::onPlaylistUpdateStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        loadPlaylistUi();
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}

void VimeoPlaylistWindow::onUserStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        loadUserUi();
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
}

void VimeoPlaylistWindow::onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status status) {
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
