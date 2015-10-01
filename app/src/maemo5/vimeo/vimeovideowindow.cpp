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

#include "vimeovideowindow.h"
#include "clipboard.h"
#include "commentdelegate.h"
#include "definitions.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "videodelegate.h"
#include "videoplaybackwindow.h"
#include "videothumbnail.h"
#include "vimeo.h"
#include "vimeocommentdialog.h"
#include "vimeocommentmodel.h"
#include "vimeodownloaddialog.h"
#include "vimeoplaybackdialog.h"
#include "vimeoplaylistdialog.h"
#include "vimeoplaylistwindow.h"
#include "vimeouser.h"
#include "vimeouserwindow.h"
#include <qvimeo/urls.h>
#include <QScrollArea>
#include <QTabBar>
#include <QStackedWidget>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

VimeoVideoWindow::VimeoVideoWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new VimeoVideo(this)),
    m_user(new VimeoUser(this)),
    m_relatedModel(new VimeoVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new VideoDelegate(m_cache, VimeoVideoModel::DateRole, VimeoVideoModel::DurationRole,
                                        VimeoVideoModel::ThumbnailUrlRole, VimeoVideoModel::TitleRole,
                                        VimeoVideoModel::UsernameRole, this)),
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
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
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
    connect(m_video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onVideoStatusChanged(QVimeo::ResourcesRequest::Status)));
            
    m_video->loadVideo(id);
}

VimeoVideoWindow::VimeoVideoWindow(const VimeoVideo *video, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new VimeoVideo(video, this)),
    m_user(new VimeoUser(this)),
    m_relatedModel(new VimeoVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new VideoDelegate(m_cache, VimeoVideoModel::DateRole, VimeoVideoModel::DurationRole,
                                        VimeoVideoModel::ThumbnailUrlRole, VimeoVideoModel::TitleRole,
                                        VimeoVideoModel::UsernameRole, this)),
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
    m_reloadAction(new QAction(tr("Reload"), this)),
    m_downloadAction(new QAction(tr("Download"), this)),
    m_shareAction(new QAction(tr("Copy URL"), this)),
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
    connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
            
    m_user->loadUser(video->userId());
}

VimeoVideoWindow::~VimeoVideoWindow() {
    delete m_cache;
    m_cache = 0;
}

void VimeoVideoWindow::loadBaseUi() {
    setWindowTitle(tr("Video info"));
    setCentralWidget(new QWidget);
    
    m_relatedView->setModel(m_relatedModel);
    m_relatedView->setItemDelegate(m_relatedDelegate);
    m_relatedView->setContextMenuPolicy(Qt::CustomContextMenu);
    
    m_thumbnail->setFixedSize(320, 240);
    m_avatar->setFixedSize(60, 60);
    
    m_titleLabel->setWordWrap(true);
    m_dateLabel->setStyleSheet(QString("color: %1; font-size: 13pt").arg(palette().color(QPalette::Mid).name()));
    m_userLabel->setStyleSheet("font-size: 13pt");

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

    menuBar()->addAction(m_reloadAction);
    menuBar()->addAction(m_downloadAction);
    menuBar()->addAction(m_shareAction);
    
    connect(m_relatedModel, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onRelatedModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_relatedView, SIGNAL(activated(QModelIndex)), this, SLOT(showRelatedVideo(QModelIndex)));
    connect(m_relatedView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_relatedDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playRelatedVideo(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playVideo()));
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showUser()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    connect(m_relatedDownloadAction, SIGNAL(triggered()), this, SLOT(downloadRelatedVideo()));
    connect(m_relatedShareAction, SIGNAL(triggered()), this, SLOT(shareRelatedVideo()));
    
    if (!Vimeo::instance()->userId().isEmpty()) {
        bool interact = Vimeo::instance()->hasScope(QVimeo::INTERACT_SCOPE);
        
        if (interact) {
            m_favouriteAction = new QAction(tr("Like"), this);
            m_watchLaterAction = new QAction(tr("Watch later"), this);
            m_relatedFavouriteAction = new QAction(this);
            m_relatedWatchLaterAction = new QAction(tr("Watch later"), this);
            menuBar()->addAction(m_favouriteAction);
            menuBar()->addAction(m_watchLaterAction);
            m_contextMenu->addAction(m_relatedFavouriteAction);
            m_contextMenu->addAction(m_relatedWatchLaterAction);
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
            connect(m_watchLaterAction, SIGNAL(triggered()), this, SLOT(watchVideoLater()));
            connect(m_relatedFavouriteAction, SIGNAL(triggered()), this, SLOT(setRelatedVideoFavourite()));
            connect(m_relatedWatchLaterAction, SIGNAL(triggered()), this, SLOT(watchRelatedVideoLater()));
        }
        
        if (Vimeo::instance()->hasScope(QVimeo::EDIT_SCOPE)) {
            m_playlistAction = new QAction(tr("Add to album"), this);
            m_relatedPlaylistAction = new QAction(tr("Add to album"), this);
            menuBar()->addAction(m_playlistAction);
            m_contextMenu->addAction(m_relatedPlaylistAction);
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));
            connect(m_relatedPlaylistAction, SIGNAL(triggered()), this, SLOT(addRelatedVideoToPlaylist()));
        }
        
        if (interact) {
            m_commentAction = new QAction(tr("Add comment"), this);
            menuBar()->addAction(m_commentAction);
            connect(m_commentAction, SIGNAL(triggered()), this, SLOT(addComment()));
        }
    }
}

void VimeoVideoWindow::loadUserUi() {
    m_userLabel->setText(m_userLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_avatar->setSource(m_user->thumbnailUrl());
}

void VimeoVideoWindow::loadVideoUi() {
    m_titleLabel->setText(m_video->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_video->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_video->description()));
    m_thumbnail->setSource(m_video->largeThumbnailUrl());
    m_thumbnail->setDuration(m_video->duration());
    
    if (m_favouriteAction) {
        m_favouriteAction->setText(m_video->isFavourite() ? tr("Unlike") : tr("Like"));
    }
}

void VimeoVideoWindow::getRelatedVideos() {
    QVariantMap filters;
    filters["per_page"] = MAX_RESULTS;
    filters["filter"] = "related";
    
    m_relatedModel->list(QString("/videos/%1/videos").arg(m_video->id()), filters);
}

void VimeoVideoWindow::addComment() {
    if (isBusy()) {
        return;
    }
    
    VimeoCommentDialog *dialog = new VimeoCommentDialog(m_video->id(), this);
    dialog->open();
}

void VimeoVideoWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    VimeoPlaylistDialog *dialog = new VimeoPlaylistDialog(m_video, this);
    dialog->open();
}

void VimeoVideoWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    VimeoDownloadDialog *dialog = new VimeoDownloadDialog(m_video->id(), m_video->title(), this);
    dialog->open();
}

void VimeoVideoWindow::playVideo() {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
        window->show();
        window->addVideo(m_video);
    }
    else {
        VimeoPlaybackDialog *dialog = new VimeoPlaybackDialog(m_video->id(), m_video->title(), this);
        dialog->open();
    }
}

void VimeoVideoWindow::setVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
    
    if (m_video->isFavourite()) {
        m_video->unfavourite();
    }
    else {
        m_video->favourite();
    }
}

void VimeoVideoWindow::shareVideo() {
    Clipboard::instance()->setText(m_video->url().toString());
    QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
}

void VimeoVideoWindow::watchVideoLater() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
    m_video->watchLater();
}

void VimeoVideoWindow::addRelatedVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        VimeoPlaylistDialog *dialog = new VimeoPlaylistDialog(video, this);
        dialog->open();
    }
}

void VimeoVideoWindow::downloadRelatedVideo() {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        QString id = m_relatedView->currentIndex().data(VimeoVideoModel::IdRole).toString();
        QString title = m_relatedView->currentIndex().data(VimeoVideoModel::TitleRole).toString();
        
        VimeoDownloadDialog *dialog = new VimeoDownloadDialog(id, title, this);
        dialog->open();
    }
}

void VimeoVideoWindow::playRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (VimeoVideo *video = m_relatedModel->get(index.row())) {
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

void VimeoVideoWindow::setRelatedVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
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

void VimeoVideoWindow::shareRelatedVideo() {
    if (const VimeoVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void VimeoVideoWindow::showRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_relatedModel->get(index.row())) {
        VimeoVideoWindow *window = new VimeoVideoWindow(video, this);
        window->show();
    }
}

void VimeoVideoWindow::watchRelatedVideoLater() {
    if (isBusy()) {
        return;
    }
    
    if (VimeoVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        connect(video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
        video->watchLater();
    }
}

void VimeoVideoWindow::reload() {
    if (m_tabBar->currentIndex() == 1) {
        if (m_commentModel) {
            m_commentModel->reload();
        }
    }
    else {
        m_relatedModel->reload();
    }
}

void VimeoVideoWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        if (m_relatedFavouriteAction) {
            m_relatedFavouriteAction->setText(m_relatedView->currentIndex().data(VimeoVideoModel::FavouriteRole).toBool()
                                              ? tr("Unlike") : tr("Like"));
        }
        
        m_contextMenu->popup(pos, m_relatedDownloadAction);
    }
}

void VimeoVideoWindow::showComments() {
    if (!m_commentView) {
        m_commentView = new ListView(this);
        m_commentDelegate = new CommentDelegate(m_cache, VimeoCommentModel::BodyRole, VimeoCommentModel::DateRole,
                                                VimeoCommentModel::ThumbnailUrlRole, VimeoCommentModel::UsernameRole,
                                                this);
        m_commentModel = new VimeoCommentModel(this);
        m_commentView->setUniformItemSizes(false);
        m_commentView->setModel(m_commentModel);
        m_commentView->setItemDelegate(m_commentDelegate);
        m_noCommentsLabel = new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No comments found")), this);
        m_stack->addWidget(m_commentView);
        m_stack->addWidget(m_noCommentsLabel);
        
        connect(m_commentDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));
        connect(m_commentModel, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onCommentModelStatusChanged(QVimeo::ResourcesRequest::Status)));
                
        QVariantMap filters;
        filters["per_page"] = MAX_RESULTS;
        m_commentModel->list(QString("/videos/%1/comments").arg(m_video->id()), filters);
    }
    
    if ((m_commentModel->rowCount() == 0) && (m_commentModel->status() != QVimeo::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noCommentsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_commentView);
    }
}

void VimeoVideoWindow::showRelatedVideos() {
    if ((m_relatedModel->rowCount() == 0) && (m_relatedModel->status() != QVimeo::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noVideosLabel);
    }
    else {
        m_stack->setCurrentWidget(m_relatedView);
    }
}

void VimeoVideoWindow::showResource(const QUrl &url) {
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

void VimeoVideoWindow::showUser() {
    if (isBusy()) {
        return;
    }
    
    VimeoUserWindow *window = new VimeoUserWindow(m_user, this);
    window->show();
}

void VimeoVideoWindow::showUser(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    VimeoUserWindow *window = new VimeoUserWindow(index.data(VimeoCommentModel::UserIdRole).toString(),
                                                              this);
    window->show();
}

void VimeoVideoWindow::onImageReady() {
    if ((m_commentView) && (m_commentView->isVisible())) {
        m_commentView->viewport()->update(m_commentView->viewport()->rect());
    }
    else {
        m_relatedView->viewport()->update(m_relatedView->viewport()->rect());
    }
}

void VimeoVideoWindow::onCommentModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        showComments();
        m_reloadAction->setEnabled(false);
        return;
    case QVimeo::ResourcesRequest::Failed:
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

void VimeoVideoWindow::onRelatedModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        showRelatedVideos();
        m_reloadAction->setEnabled(false);
        return;
    case QVimeo::ResourcesRequest::Failed:
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

void VimeoVideoWindow::onTabIndexChanged(int index) {
    if (index == 1) {
        showComments();
    }
    else {
        showRelatedVideos();
    }
}

void VimeoVideoWindow::onUserStatusChanged(QVimeo::ResourcesRequest::Status status) {
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

void VimeoVideoWindow::onVideoStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        loadVideoUi();
        getRelatedVideos();
        connect(m_user, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(QVimeo::ResourcesRequest::Status)));
        m_user->loadUser(m_video->userId());
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_video->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
               this, SLOT(onVideoStatusChanged(QVimeo::ResourcesRequest::Status)));
}

void VimeoVideoWindow::onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status status) {
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
    
    if (video == m_video) {
        loadVideoUi();
    }
    
    hideProgressIndicator();
    disconnect(video, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
               this, SLOT(onVideoUpdateStatusChanged(QVimeo::ResourcesRequest::Status)));
}
