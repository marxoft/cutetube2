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

#include "youtubevideowindow.h"
#include "clipboard.h"
#include "definitions.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "videoplaybackwindow.h"
#include "videothumbnail.h"
#include "youtube.h"
#include "youtubecommentdelegate.h"
#include "youtubecommentdialog.h"
#include "youtubecommentmodel.h"
#include "youtubedownloaddialog.h"
#include "youtubeplaybackdialog.h"
#include "youtubeplaylistdialog.h"
#include "youtubeplaylistwindow.h"
#include "youtubeuser.h"
#include "youtubeuserwindow.h"
#include "youtubevideodelegate.h"
#include <qyoutube/urls.h>
#include <QScrollArea>
#include <QTabBar>
#include <QStackedWidget>
#include <QLabel>
#include <QActionGroup>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

YouTubeVideoWindow::YouTubeVideoWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new YouTubeVideo(this)),
    m_user(new YouTubeUser(this)),
    m_relatedModel(new YouTubeVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new YouTubeVideoDelegate(m_cache, this)),
    m_commentDelegate(0),
    m_scrollArea(new QScrollArea(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_userLabel(new QLabel(this)),
    m_noVideosLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this)),
    m_noCommentsLabel(0),
    m_viewGroup(new QActionGroup(this)),
    m_listAction(new QAction(tr("List"), this)),
    m_gridAction(new QAction(tr("Grid"), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_likeAction(0),
    m_dislikeAction(0),
    m_favouriteAction(0),
    m_watchLaterAction(0),
    m_playlistAction(0),
    m_commentAction(0),
    m_contextMenu(new QMenu(this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this)),
    m_relatedFavouriteAction(0),
    m_relatedWatchLaterAction(0),
    m_relatedPlaylistAction(0)
{
    loadBaseUi();
    connect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onVideoStatusChanged(QYouTube::ResourcesRequest::Status)));
            
    m_video->loadVideo(id);
}

YouTubeVideoWindow::YouTubeVideoWindow(const YouTubeVideo *video, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new YouTubeVideo(video, this)),
    m_user(new YouTubeUser(this)),
    m_relatedModel(new YouTubeVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new YouTubeVideoDelegate(m_cache, this)),
    m_commentDelegate(0),
    m_scrollArea(new QScrollArea(this)),
    m_tabBar(new QTabBar(this)),
    m_stack(new QStackedWidget(this)),
    m_titleLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_dateLabel(new QLabel(this)),
    m_userLabel(new QLabel(this)),
    m_noVideosLabel(new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No videos found")), this)),
    m_noCommentsLabel(0),
    m_viewGroup(new QActionGroup(this)),
    m_listAction(new QAction(tr("List"), this)),
    m_gridAction(new QAction(tr("Grid"), this)),
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_likeAction(0),
    m_dislikeAction(0),
    m_favouriteAction(0),
    m_watchLaterAction(0),
    m_playlistAction(0),
    m_commentAction(0),
    m_contextMenu(new QMenu(this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this)),
    m_relatedFavouriteAction(0),
    m_relatedWatchLaterAction(0),
    m_relatedPlaylistAction(0)
{
    loadBaseUi();
    loadVideoUi();
    getRelatedVideos();
    connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
            SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
            
    m_user->loadUser(video->userId());
}

YouTubeVideoWindow::~YouTubeVideoWindow() {
    delete m_cache;
    m_cache = 0;
}

void YouTubeVideoWindow::loadBaseUi() {
    setWindowTitle(tr("Video info"));
    setCentralWidget(new QWidget);
    
    m_relatedView->setModel(m_relatedModel);
    m_relatedView->setItemDelegate(m_relatedDelegate);
    m_relatedView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 240);
    m_avatar->setFixedSize(60, 60);
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 18px").arg(palette().color(QPalette::Mid).name()));
    m_userLabel->setStyleSheet("font-size: 18px");
    
    m_listAction->setCheckable(true);
    m_listAction->setChecked(true);
    m_gridAction->setCheckable(true);
    m_viewGroup->setExclusive(true);
    m_viewGroup->addAction(m_listAction);
    m_viewGroup->addAction(m_gridAction);
    m_reloadAction->setEnabled(false);
    
    m_contextMenu->addAction(m_relatedDownloadAction);
    m_contextMenu->addAction(m_relatedShareAction);
    
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
    
    m_tabBar->addTab(tr("Related"));
    m_tabBar->addTab(tr("Comments"));
    m_tabBar->setExpanding(false);
    m_tabBar->setDrawBase(false);
    m_tabBar->setStyleSheet("QTabBar::tab { height: 40px; }");
    
    m_stack->addWidget(m_relatedView);
    m_stack->addWidget(m_noVideosLabel);
    
    m_layout = new QGridLayout(centralWidget());
    m_layout->addWidget(m_scrollArea, 0, 0, 2, 1);
    m_layout->addWidget(m_tabBar, 0, 1);
    m_layout->addWidget(m_stack, 1, 1);
    m_layout->setRowStretch(1, 1);
    m_layout->setColumnStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    menuBar()->addAction(m_listAction);
    menuBar()->addAction(m_gridAction);
    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_downloadAction);
    menuBar()->addAction(m_shareAction);
    
    connect(m_relatedModel, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
            SLOT(onRelatedModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_relatedView, SIGNAL(activated(QModelIndex)), this, SLOT(showRelatedVideo(QModelIndex)));
    connect(m_relatedView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_relatedDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playRelatedVideo(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_listAction, SIGNAL(triggered()), this, SLOT(enableListMode()));
    connect(m_gridAction, SIGNAL(triggered()), this, SLOT(enableGridMode()));
    connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playVideo()));
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showUser()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    connect(m_relatedDownloadAction, SIGNAL(triggered()), this, SLOT(downloadRelatedVideo()));
    connect(m_relatedShareAction, SIGNAL(triggered()), this, SLOT(shareRelatedVideo()));
    
    if (!YouTube::instance()->userId().isEmpty()) {
        if ((YouTube::instance()->hasScope(QYouTube::READ_WRITE_SCOPE))
            || (YouTube::instance()->hasScope(QYouTube::FORCE_SSL_SCOPE))) {
                        
            m_likeAction = new QAction(tr("Like"), this);
            m_dislikeAction = new QAction(tr("Dislike"), this);
            m_favouriteAction = new QAction(tr("Favourite"), this);
            m_watchLaterAction = new QAction(tr("Watch later"), this);
            m_playlistAction = new QAction(tr("Add to playlist"), this);
            m_relatedFavouriteAction = new QAction(this);
            m_relatedWatchLaterAction = new QAction(tr("Watch later"), this);
            m_relatedPlaylistAction = new QAction(tr("Add to playlist"), this);
            menuBar()->addAction(m_likeAction);
            menuBar()->addAction(m_dislikeAction);
            menuBar()->addAction(m_favouriteAction);
            menuBar()->addAction(m_watchLaterAction);
            menuBar()->addAction(m_playlistAction);
            m_contextMenu->addAction(m_relatedFavouriteAction);
            m_contextMenu->addAction(m_relatedWatchLaterAction);
            m_contextMenu->addAction(m_relatedPlaylistAction);
            connect(m_likeAction, SIGNAL(triggered()), this, SLOT(likeVideo()));
            connect(m_dislikeAction, SIGNAL(triggered()), this, SLOT(dislikeVideo()));
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
            connect(m_watchLaterAction, SIGNAL(triggered()), this, SLOT(watchVideoLater()));
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));
            connect(m_relatedFavouriteAction, SIGNAL(triggered()), this, SLOT(setRelatedVideoFavourite()));
            connect(m_relatedWatchLaterAction, SIGNAL(triggered()), this, SLOT(watchRelatedVideoLater()));
            connect(m_relatedPlaylistAction, SIGNAL(triggered()), this, SLOT(addRelatedVideoToPlaylist()));
            
            if (YouTube::instance()->hasScope(QYouTube::FORCE_SSL_SCOPE)) {
                m_commentAction = new QAction(tr("Add comment"), this);
                menuBar()->addAction(m_commentAction);
                connect(m_commentAction, SIGNAL(triggered()), this, SLOT(addComment()));
            }
        }
    }
    
    if (Settings::instance()->defaultViewMode() == "grid") {
        m_gridAction->trigger();
    }
}

void YouTubeVideoWindow::loadUserUi() {
    m_userLabel->setText(m_user->username());
    m_avatar->setSource(m_user->thumbnailUrl());
}

void YouTubeVideoWindow::loadVideoUi() {
    m_titleLabel->setText(m_video->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_video->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_video->description()));
    m_thumbnail->setSource(m_video->largeThumbnailUrl());
    m_thumbnail->setDuration(m_video->duration());
    
    if (m_likeAction) {
        m_likeAction->setText(tr("Like (%1)").arg(m_video->likeCount()));
    }
    
    if (m_dislikeAction) {
        m_dislikeAction->setText(tr("Dislike (%1)").arg(m_video->dislikeCount()));
    }
    
    if (m_favouriteAction) {
        m_favouriteAction->setText(m_video->isFavourite() ? tr("Unfavourite") : tr("Favourite"));
    }
}

void YouTubeVideoWindow::enableGridMode() {
    m_relatedDelegate->setGridMode(true);
    m_relatedView->setFlow(QListView::LeftToRight);
    m_relatedView->setWrapping(true);
    Settings::instance()->setDefaultViewMode("grid");
}

void YouTubeVideoWindow::enableListMode() {
    m_relatedDelegate->setGridMode(false);
    m_relatedView->setFlow(QListView::TopToBottom);
    m_relatedView->setWrapping(false);
    Settings::instance()->setDefaultViewMode("list");
}

void YouTubeVideoWindow::getRelatedVideos() {
    QVariantMap filters;
    filters["relatedToVideoId"] = m_video->id();
    
    QVariantMap params;
    params["type"] = "video";
    params["maxResults"] = MAX_RESULTS;
    
    m_relatedModel->list("/search", QStringList() << "snippet", filters, params);
}

void YouTubeVideoWindow::addComment() {
    if (isBusy()) {
        return;
    }
    
    YouTubeCommentDialog *dialog = new YouTubeCommentDialog(m_video->id(), QString(), this);
    dialog->open();
}

void YouTubeVideoWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    YouTubePlaylistDialog *dialog = new YouTubePlaylistDialog(m_video, this);
    dialog->open();
}

void YouTubeVideoWindow::dislikeVideo() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    m_video->dislike();
}

void YouTubeVideoWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    YouTubeDownloadDialog *dialog = new YouTubeDownloadDialog(m_video->id(), m_video->title(), this);
    dialog->open();
}

void YouTubeVideoWindow::likeVideo() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    m_video->like();
}

void YouTubeVideoWindow::playVideo() {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
        window->show();
        window->addVideo(m_video);
    }
    else {
        YouTubePlaybackDialog *dialog = new YouTubePlaybackDialog(m_video->id(), m_video->title(), this);
        dialog->open();
    }
}

void YouTubeVideoWindow::shareVideo() {
    Clipboard::instance()->setText(m_video->url().toString());
    QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
}

void YouTubeVideoWindow::setVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    
    if (m_video->isFavourite()) {
        m_video->unfavourite();
    }
    else {
        m_video->favourite();
    }
}

void YouTubeVideoWindow::watchVideoLater() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
    m_video->watchLater();
}

void YouTubeVideoWindow::addRelatedVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        YouTubePlaylistDialog *dialog = new YouTubePlaylistDialog(video, this);
        dialog->open();
    }
}

void YouTubeVideoWindow::downloadRelatedVideo() {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        QString id = m_relatedView->currentIndex().data(YouTubeVideoModel::IdRole).toString();
        QString title = m_relatedView->currentIndex().data(YouTubeVideoModel::TitleRole).toString();
        
        YouTubeDownloadDialog *dialog = new YouTubeDownloadDialog(id, title, this);
        dialog->open();
    }
}

void YouTubeVideoWindow::playRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (const YouTubeVideo *video = m_relatedModel->get(index.row())) {
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

void YouTubeVideoWindow::setRelatedVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
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

void YouTubeVideoWindow::shareRelatedVideo() {
    if (const YouTubeVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("Copy URL to clipboard"));
    }
}

void YouTubeVideoWindow::showRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_relatedModel->get(index.row())) {
        YouTubeVideoWindow *window = new YouTubeVideoWindow(video, this);
        window->show();
    }
}

void YouTubeVideoWindow::watchRelatedVideoLater() {
    if (isBusy()) {
        return;
    }
    
    if (YouTubeVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
        video->watchLater();      
    }
}

void YouTubeVideoWindow::reload() {
    if (m_tabBar->currentIndex() == 1) {
        if (m_commentModel) {
            m_commentModel->reload();
        }
    }
    else {
        m_relatedModel->reload();
    }
}

void YouTubeVideoWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        if (m_relatedFavouriteAction) {
            m_relatedFavouriteAction->setText(m_relatedView->currentIndex().data(YouTubeVideoModel::FavouriteRole).toBool()
                                              ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_relatedDownloadAction);
    }
}

void YouTubeVideoWindow::showComments() {
    if (!m_commentView) {
        m_commentView = new ListView(this);
        m_commentDelegate = new YouTubeCommentDelegate(m_cache, this);
        m_commentModel = new YouTubeCommentModel(this);
        m_commentView->setUniformItemSizes(false);
        m_commentView->setModel(m_commentModel);
        m_commentView->setItemDelegate(m_commentDelegate);
        m_noCommentsLabel = new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No comments found")), this);
        m_stack->addWidget(m_commentView);
        m_stack->addWidget(m_noCommentsLabel);
        
        connect(m_commentDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));
        connect(m_commentModel, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onCommentModelStatusChanged(QYouTube::ResourcesRequest::Status)));
        
        QVariantMap filters;
        filters["videoId"] = m_video->id();
        
        QVariantMap params;
        params["maxResults"] = MAX_RESULTS;
        params["textFormat"] = "plainText";
        
        m_commentModel->list("/commentThreads", QStringList() << "snippet" << "replies", filters, params);
    }
    
    if ((m_commentModel->rowCount() == 0) && (m_commentModel->status() != QYouTube::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noCommentsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_commentView);
    }
}

void YouTubeVideoWindow::showRelatedVideos() {
    if ((m_relatedModel->rowCount() == 0) && (m_relatedModel->status() != QYouTube::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noVideosLabel);
    }
    else {
        m_stack->setCurrentWidget(m_relatedView);
    }
}

void YouTubeVideoWindow::showResource(const QUrl &url) {
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

void YouTubeVideoWindow::showUser() {
    if (isBusy()) {
        return;
    }
    
    YouTubeUserWindow *window = new YouTubeUserWindow(m_user, this);
    window->show();
}

void YouTubeVideoWindow::showUser(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    YouTubeUserWindow *window = new YouTubeUserWindow(index.data(YouTubeCommentModel::UserIdRole).toString(),
                                                              this);
    window->show();
}

void YouTubeVideoWindow::onImageReady() {
    if ((m_commentView) && (m_commentView->isVisible())) {
        m_commentView->viewport()->update(m_commentView->viewport()->rect());
    }
    else {
        m_relatedView->viewport()->update(m_relatedView->viewport()->rect());
    }
}

void YouTubeVideoWindow::onCommentModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        showComments();
        m_reloadAction->setEnabled(false);
        return;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_commentModel->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_commentView->isVisible()) {
        showComments();
    }
}

void YouTubeVideoWindow::onRelatedModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        showRelatedVideos();
        m_reloadAction->setEnabled(false);
        return;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_relatedModel->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    m_reloadAction->setEnabled(true);
    
    if (m_relatedView->isVisible()) {
        showRelatedVideos();
    }
}

void YouTubeVideoWindow::onTabIndexChanged(int index) {
    if (index == 1) {
        showComments();
    }
    else {
        showRelatedVideos();
    }
}

void YouTubeVideoWindow::onUserStatusChanged(QYouTube::ResourcesRequest::Status status) {
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

void YouTubeVideoWindow::onVideoStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        loadVideoUi();
        getRelatedVideos();
        connect(m_user, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(QYouTube::ResourcesRequest::Status)));
        m_user->loadUser(m_video->userId());
        break;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_video->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
               this, SLOT(onVideoStatusChanged(QYouTube::ResourcesRequest::Status)));
}

void YouTubeVideoWindow::onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status status) {
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
    
    if (video == m_video) {
        loadVideoUi();
    }
    
    hideProgressIndicator();
    disconnect(video, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
               this, SLOT(onVideoUpdateStatusChanged(QYouTube::ResourcesRequest::Status)));
}
