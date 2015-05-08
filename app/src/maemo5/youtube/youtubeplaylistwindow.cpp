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

#include "youtubeplaylistwindow.h"
#include "clipboard.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "playlistthumbnail.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "videoplaybackwindow.h"
#include "youtube.h"
#include "youtubedownloaddialog.h"
#include "youtubeplaybackdialog.h"
#include "youtubeplaylistdialog.h"
#include "youtubeplaylistwindow.h"
#include "youtubeuser.h"
#include "youtubeuserwindow.h"
#include "youtubevideodelegate.h"
#include "youtubevideomodel.h"
#include "youtubevideowindow.h"
#include <qyoutube/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QActionGroup>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

YouTubePlaylistWindow::YouTubePlaylistWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new YouTubePlaylist(this)),
    m_user(new YouTubeUser(this)),
    m_model(new YouTubeVideoModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new PlaylistThumbnail(this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_delegate(new YouTubeVideoDelegate(m_cache, this)),
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
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_watchLaterAction(0),
    m_playlistAction(0),
    m_removeAction(0)
{
    loadBaseUi();
    connect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status)));
            
    m_playlist->loadPlaylist(id);
}

YouTubePlaylistWindow::YouTubePlaylistWindow(const YouTubePlaylist *playlist, StackedWindow *parent) :
    StackedWindow(parent),
    m_playlist(new YouTubePlaylist(playlist, this)),
    m_user(new YouTubeUser(this)),
    m_model(new YouTubeVideoModel(this)),
    m_cache(new ImageCache),
    m_thumbnail(new PlaylistThumbnail(this)),
    m_avatar(new Image(this)),
    m_view(new ListView(this)),
    m_delegate(new YouTubeVideoDelegate(m_cache, this)),
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
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_favouriteAction(0),
    m_watchLaterAction(0),
    m_playlistAction(0),
    m_removeAction(0)
{
    loadBaseUi();
    loadPlaylistUi();
    getVideos();
    connect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onPlaylistUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
            SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
    
    m_user->loadUser(playlist->userId());
}

YouTubePlaylistWindow::~YouTubePlaylistWindow() {
    delete m_cache;
    m_cache = 0;
}

void YouTubePlaylistWindow::loadBaseUi() {
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
    
    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
            SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_listAction, SIGNAL(triggered()), this, SLOT(enableListMode()));
    connect(m_gridAction, SIGNAL(triggered()), this, SLOT(enableGridMode()));
    connect(m_reloadAction, SIGNAL(triggered()), m_model, SLOT(reload()));
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showUser()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    
    if (!YouTube::instance()->userId().isEmpty()) {
        if ((YouTube::instance()->hasScope(QYouTube::READ_WRITE_SCOPE))
            || (YouTube::instance()->hasScope(QYouTube::FORCE_SSL_SCOPE))) {
            m_favouriteAction = new QAction(this);
            m_watchLaterAction = new QAction(tr("Watch later"), this);
            m_playlistAction = new QAction(tr("Add to playlist"), this);
            m_contextMenu->addAction(m_favouriteAction);
            m_contextMenu->addAction(m_watchLaterAction);
            m_contextMenu->addAction(m_playlistAction);
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
            connect(m_watchLaterAction, SIGNAL(triggered()), this, SLOT(watchVideoLater()));
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));
        }    
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playPlaylist()));
    }
    
    if (Settings::instance()->defaultViewMode() == "grid") {
        m_gridAction->trigger();
    }
}

void YouTubePlaylistWindow::loadPlaylistUi() {
    m_titleLabel->setText(m_playlist->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_playlist->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_playlist->description()));
    m_thumbnail->setSource(m_playlist->largeThumbnailUrl());
    m_thumbnail->setVideoCount(m_playlist->videoCount());
    
    if ((!m_removeAction) && (m_playlist->userId() == YouTube::instance()->userId())
        && ((YouTube::instance()->hasScope(QYouTube::READ_WRITE_SCOPE))
        || (YouTube::instance()->hasScope(QYouTube::FORCE_SSL_SCOPE)))) {
        m_removeAction = new QAction(tr("Remove"), this);
        m_contextMenu->addAction(m_removeAction);
        connect(m_removeAction, SIGNAL(triggered()), this, SLOT(removeVideo()));
    }
}

void YouTubePlaylistWindow::loadUserUi() {
    m_userLabel->setText(m_user->username());
    m_avatar->setSource(m_user->thumbnailUrl());
}

void YouTubePlaylistWindow::enableGridMode() {
    m_delegate->setGridMode(true);
    m_view->setFlow(QListView::LeftToRight);
    m_view->setWrapping(true);
    Settings::instance()->setDefaultViewMode("grid");
}

void YouTubePlaylistWindow::enableListMode() {
    m_delegate->setGridMode(false);
    m_view->setFlow(QListView::TopToBottom);
    m_view->setWrapping(false);
    Settings::instance()->setDefaultViewMode("list");
}

void YouTubePlaylistWindow::getVideos() {
    QVariantMap filters;
    filters["playlistId"] = m_playlist->id();
    
    QVariantMap params;
    params["maxResults"] = 50;
    
    m_model->list("/playlistItems", QStringList() << "snippet", filters, params);
}

void YouTubePlaylistWindow::playPlaylist() {
    if ((isBusy()) || (m_model->rowCount() == 0)) {
        return;
    }
    
    QList<const Video*> list;
    
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (const Video *video = m_model->get(i)) {
            list << video;
        }
    }
    
    if (!list.isEmpty()) {
        VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
        window->show();
        window->addVideos(list);
    }
}

void YouTubePlaylistWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (const YouTubeVideo *video = m_model->get(m_view->currentIndex().row())) {
        YouTubePlaylistDialog *dialog = new YouTubePlaylistDialog(video, this);
        dialog->open();
    }
}

void YouTubePlaylistWindow::downloadVideo() {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        QString id = m_view->currentIndex().data(YouTubeVideoModel::IdRole).toString();
        QString title = m_view->currentIndex().data(YouTubeVideoModel::TitleRole).toString();
        
        YouTubeDownloadDialog *dialog = new YouTubeDownloadDialog(id, title, this);
        dialog->open();
    }
}

void YouTubePlaylistWindow::playVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (const YouTubeVideo *video = m_model->get(index.row())) {
            VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
            window->show();
            window->addVideo(video);
        }
    }
    else {
        QString id = index.data(YouTubeVideoModel::IdRole).toString();
        QString title = index.data(YouTubeVideoModel::TitleRole).toString();
    
        YouTubePlaybackDialog *dialog = new YouTubePlaybackDialog(id, title, this);
        dialog->open();
    }
}

void YouTubePlaylistWindow::removeVideo() {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_model->get(m_view->currentIndex().row())) {
        m_playlist->removeVideo(video);
    }
}

void YouTubePlaylistWindow::setVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_model->get(m_view->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
        
        if (video->isFavourite()) {
            video->unfavourite();
        }
        else {
            video->favourite();
        }
    }
}

void YouTubePlaylistWindow::shareVideo() {
    if (const YouTubeVideo *video = m_model->get(m_view->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void YouTubePlaylistWindow::showVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (const YouTubeVideo *video = m_model->get(index.row())) {
        YouTubeVideoWindow *window = new YouTubeVideoWindow(video, this);
        window->show();
    }
}

void YouTubePlaylistWindow::watchVideoLater() {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_model->get(m_view->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
        video->watchLater();
    }
}

void YouTubePlaylistWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_view->currentIndex().isValid())) {
        if (m_favouriteAction) {
            m_favouriteAction->setText(m_view->currentIndex().data(YouTubeVideoModel::FavouriteRole).toBool()
                                       ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_downloadAction);
    }
}

void YouTubePlaylistWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::YOUTUBE) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        YouTubePlaylistWindow *window = new YouTubePlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        YouTubeUserWindow *window = new YouTubeUserWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        YouTubeVideoWindow *window = new YouTubeVideoWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void YouTubePlaylistWindow::showUser() {
    YouTubeUserWindow *window = new YouTubeUserWindow(m_user, this);
    window->show();
}

void YouTubePlaylistWindow::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void YouTubePlaylistWindow::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        m_noVideosLabel->hide();
        m_view->show();
        m_reloadAction->setEnabled(false);
        return;
    case QYouTube::ResourcesRequest::Ready:
        if (m_model->rowCount() > 0) {
            m_model->fetchMore();
        }
        
        break;
    case QYouTube::ResourcesRequest::Failed:
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

void YouTubePlaylistWindow::onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        loadPlaylistUi();
        getVideos();
        connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
        m_user->loadUser(m_playlist->userId());
        break;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
               this, SLOT(onPlaylistStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_playlist, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onPlaylistUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
}

void YouTubePlaylistWindow::onPlaylistUpdateStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        loadPlaylistUi();
        break;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_playlist->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}

void YouTubePlaylistWindow::onUserStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        loadUserUi();
        break;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
}

void YouTubePlaylistWindow::onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status status) {
    const YouTubeVideo *video = qobject_cast<YouTubeVideo*>(sender());
    
    if (!video) {
        return;
    }
    
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), video->errorString());
        break;
    default:
        break;
    }

    hideProgressIndicator();
    disconnect(video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
               this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
}
