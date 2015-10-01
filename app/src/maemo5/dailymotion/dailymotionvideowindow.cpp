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

#include "dailymotionvideowindow.h"
#include "clipboard.h"
#include "commentdelegate.h"
#include "dailymotion.h"
#include "dailymotioncommentdialog.h"
#include "dailymotioncommentmodel.h"
#include "dailymotiondownloaddialog.h"
#include "dailymotionplaybackdialog.h"
#include "dailymotionplaylistdialog.h"
#include "dailymotionplaylistwindow.h"
#include "dailymotionuser.h"
#include "dailymotionuserwindow.h"
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
#include <qdailymotion/urls.h>
#include <QScrollArea>
#include <QTabBar>
#include <QStackedWidget>
#include <QLabel>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenuBar>
#include <QDesktopServices>
#include <QMaemo5InformationBox>

DailymotionVideoWindow::DailymotionVideoWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new DailymotionVideo(this)),
    m_user(new DailymotionUser(this)),
    m_relatedModel(new DailymotionVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new VideoDelegate(m_cache, DailymotionVideoModel::DateRole, DailymotionVideoModel::DurationRole,
                                        DailymotionVideoModel::ThumbnailUrlRole, DailymotionVideoModel::TitleRole,
                                        DailymotionVideoModel::UsernameRole, this)),
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
    m_playlistAction(0),
    m_commentAction(0),
    m_contextMenu(new QMenu(this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this)),
    m_relatedFavouriteAction(0),
    m_relatedPlaylistAction(0)
{
    loadBaseUi();
    connect(m_video, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onVideoStatusChanged(QDailymotion::ResourcesRequest::Status)));
            
    m_video->loadVideo(id);
}

DailymotionVideoWindow::DailymotionVideoWindow(const DailymotionVideo *video, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new DailymotionVideo(video, this)),
    m_user(new DailymotionUser(this)),
    m_relatedModel(new DailymotionVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new VideoDelegate(m_cache, DailymotionVideoModel::DateRole, DailymotionVideoModel::DurationRole,
                                        DailymotionVideoModel::ThumbnailUrlRole, DailymotionVideoModel::TitleRole,
                                        DailymotionVideoModel::UsernameRole, this)),
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
    m_playlistAction(0),
    m_commentAction(0),
    m_contextMenu(new QMenu(this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this)),
    m_relatedFavouriteAction(0),
    m_relatedPlaylistAction(0)
{
    loadBaseUi();
    loadVideoUi();
    getRelatedVideos();
    connect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
            SLOT(onUserStatusChanged(QDailymotion::ResourcesRequest::Status)));
            
    m_user->loadUser(video->userId());
}

DailymotionVideoWindow::~DailymotionVideoWindow() {
    delete m_cache;
    m_cache = 0;
}

void DailymotionVideoWindow::loadBaseUi() {
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
    
    connect(m_relatedModel, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
            SLOT(onRelatedModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
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
    
    if (!Dailymotion::instance()->userId().isEmpty()) {        
        if (Dailymotion::instance()->hasScope(QDailymotion::MANAGE_FAVORITES_SCOPE)) {
            m_favouriteAction = new QAction(tr("Favourite"), this);
            m_relatedFavouriteAction = new QAction(this);
            menuBar()->addAction(m_favouriteAction);
            m_contextMenu->addAction(m_relatedFavouriteAction);
            connect(m_favouriteAction, SIGNAL(triggered()), this, SLOT(setVideoFavourite()));
            connect(m_relatedFavouriteAction, SIGNAL(triggered()), this, SLOT(setRelatedVideoFavourite()));
        }
        
        if (Dailymotion::instance()->hasScope(QDailymotion::MANAGE_PLAYLISTS_SCOPE)) {
            m_playlistAction = new QAction(tr("Add to playlist"), this);
            m_relatedPlaylistAction = new QAction(tr("Add to playlist"), this);
            menuBar()->addAction(m_playlistAction);
            m_contextMenu->addAction(m_relatedPlaylistAction);
            connect(m_playlistAction, SIGNAL(triggered()), this, SLOT(addVideoToPlaylist()));
            connect(m_relatedPlaylistAction, SIGNAL(triggered()), this, SLOT(addRelatedVideoToPlaylist()));
        }
        
        if (Dailymotion::instance()->hasScope(QDailymotion::MANAGE_COMMENTS_SCOPE)) {
            m_commentAction = new QAction(tr("Add comment"), this);
            menuBar()->addAction(m_commentAction);
            connect(m_commentAction, SIGNAL(triggered()), this, SLOT(addComment()));
        }
    }
}

void DailymotionVideoWindow::loadUserUi() {
    m_userLabel->setText(m_userLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_avatar->setSource(m_user->thumbnailUrl());
}

void DailymotionVideoWindow::loadVideoUi() {
    m_titleLabel->setText(m_video->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_video->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_video->description()));
    m_thumbnail->setSource(m_video->largeThumbnailUrl());
    m_thumbnail->setDuration(m_video->duration());
    
    if (m_favouriteAction) {
        m_favouriteAction->setText(m_video->isFavourite() ? tr("Unfavourite") : tr("Favourite"));
    }
}

void DailymotionVideoWindow::getRelatedVideos() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    filters["family_filter"] = Settings::instance()->safeSearchEnabled();
    
    m_relatedModel->list(QString("/video/%1/related").arg(m_video->id()), filters);
}

void DailymotionVideoWindow::addComment() {
    if (isBusy()) {
        return;
    }
    
    DailymotionCommentDialog *dialog = new DailymotionCommentDialog(m_video->id(), this);
    dialog->open();
}

void DailymotionVideoWindow::addVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    DailymotionPlaylistDialog *dialog = new DailymotionPlaylistDialog(m_video, this);
    dialog->open();
}

void DailymotionVideoWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    DailymotionDownloadDialog *dialog = new DailymotionDownloadDialog(m_video->id(), m_video->title(), this);
    dialog->open();
}

void DailymotionVideoWindow::playVideo() {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
        window->show();
        window->addVideo(m_video);
    }
    else {
        DailymotionPlaybackDialog *dialog = new DailymotionPlaybackDialog(m_video->id(), m_video->title(), this);
        dialog->open();
    }
}

void DailymotionVideoWindow::setVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    connect(m_video, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
                this, SLOT(onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status)));
    
    if (m_video->isFavourite()) {
        m_video->unfavourite();
    }
    else {
        m_video->favourite();
    }
}

void DailymotionVideoWindow::shareVideo() {
    Clipboard::instance()->setText(m_video->url().toString());
    QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
}

void DailymotionVideoWindow::addRelatedVideoToPlaylist() {
    if (isBusy()) {
        return;
    }
    
    if (DailymotionVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        DailymotionPlaylistDialog *dialog = new DailymotionPlaylistDialog(video, this);
        dialog->open();
    }
}

void DailymotionVideoWindow::downloadRelatedVideo() {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        QString id = m_relatedView->currentIndex().data(DailymotionVideoModel::IdRole).toString();
        QString title = m_relatedView->currentIndex().data(DailymotionVideoModel::TitleRole).toString();
        
        DailymotionDownloadDialog *dialog = new DailymotionDownloadDialog(id, title, this);
        dialog->open();
    }
}

void DailymotionVideoWindow::playRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (DailymotionVideo *video = m_relatedModel->get(index.row())) {
            VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
            window->show();
            window->addVideo(video);
        }
    }
    else {
        QString id = index.data(DailymotionVideoModel::IdRole).toString();
        QString title = index.data(DailymotionVideoModel::TitleRole).toString();
    
        DailymotionPlaybackDialog *dialog = new DailymotionPlaybackDialog(id, title, this);
        dialog->open();
    }
}

void DailymotionVideoWindow::setRelatedVideoFavourite() {
    if (isBusy()) {
        return;
    }
    
    if (DailymotionVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
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

void DailymotionVideoWindow::shareRelatedVideo() {
    if (const DailymotionVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void DailymotionVideoWindow::showRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (DailymotionVideo *video = m_relatedModel->get(index.row())) {
        DailymotionVideoWindow *window = new DailymotionVideoWindow(video, this);
        window->show();
    }
}

void DailymotionVideoWindow::reload() {
    if (m_tabBar->currentIndex() == 1) {
        if (m_commentModel) {
            m_commentModel->reload();
        }
    }
    else {
        m_relatedModel->reload();
    }
}

void DailymotionVideoWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        if (m_relatedFavouriteAction) {
            m_relatedFavouriteAction->setText(m_relatedView->currentIndex().data(DailymotionVideoModel::FavouriteRole).toBool()
                                              ? tr("Unfavourite") : tr("Favourite"));
        }
        
        m_contextMenu->popup(pos, m_relatedDownloadAction);
    }
}

void DailymotionVideoWindow::showComments() {
    if (!m_commentView) {
        m_commentView = new ListView(this);
        m_commentDelegate = new CommentDelegate(m_cache, DailymotionCommentModel::BodyRole,
                                                DailymotionCommentModel::DateRole,
                                                DailymotionCommentModel::ThumbnailUrlRole,
                                                DailymotionCommentModel::UsernameRole, this);
        m_commentModel = new DailymotionCommentModel(this);
        m_commentView->setUniformItemSizes(false);
        m_commentView->setModel(m_commentModel);
        m_commentView->setItemDelegate(m_commentDelegate);
        m_noCommentsLabel = new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No comments found")), this);
        m_stack->addWidget(m_commentView);
        m_stack->addWidget(m_noCommentsLabel);
        
        connect(m_commentDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));
        connect(m_commentModel, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
                this, SLOT(onCommentModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
                
        QVariantMap filters;
        filters["limit"] = MAX_RESULTS;
        m_commentModel->list(QString("/video/%1/comments").arg(m_video->id()), filters);
    }
    
    if ((m_commentModel->rowCount() == 0) && (m_commentModel->status() != QDailymotion::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noCommentsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_commentView);
    }
}

void DailymotionVideoWindow::showRelatedVideos() {
    if ((m_relatedModel->rowCount() == 0) && (m_relatedModel->status() != QDailymotion::ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noVideosLabel);
    }
    else {
        m_stack->setCurrentWidget(m_relatedView);
    }
}

void DailymotionVideoWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::DAILYMOTION) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        DailymotionPlaylistWindow *window = new DailymotionPlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        DailymotionUserWindow *window = new DailymotionUserWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        DailymotionVideoWindow *window = new DailymotionVideoWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void DailymotionVideoWindow::showUser() {
    if (isBusy()) {
        return;
    }
    
    DailymotionUserWindow *window = new DailymotionUserWindow(m_user, this);
    window->show();
}

void DailymotionVideoWindow::showUser(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    DailymotionUserWindow *window = new DailymotionUserWindow(index.data(DailymotionCommentModel::UserIdRole).toString(),
                                                              this);
    window->show();
}

void DailymotionVideoWindow::onImageReady() {
    if ((m_commentView) && (m_commentView->isVisible())) {
        m_commentView->viewport()->update(m_commentView->viewport()->rect());
    }
    else {
        m_relatedView->viewport()->update(m_relatedView->viewport()->rect());
    }
}

void DailymotionVideoWindow::onCommentModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        showComments();
        m_reloadAction->setEnabled(false);
        return;
    case QDailymotion::ResourcesRequest::Failed:
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

void DailymotionVideoWindow::onRelatedModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        showRelatedVideos();
        m_reloadAction->setEnabled(false);
        return;
    case QDailymotion::ResourcesRequest::Failed:
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

void DailymotionVideoWindow::onTabIndexChanged(int index) {
    if (index == 1) {
        showComments();
    }
    else {
        showRelatedVideos();
    }
}

void DailymotionVideoWindow::onUserStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Ready:
        loadUserUi();
        break;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(QDailymotion::ResourcesRequest::Status)));
}

void DailymotionVideoWindow::onVideoStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Ready:
        loadVideoUi();
        getRelatedVideos();
        connect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(QDailymotion::ResourcesRequest::Status)));
        m_user->loadUser(m_video->userId());
        break;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_video->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_video, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
               this, SLOT(onVideoStatusChanged(QDailymotion::ResourcesRequest::Status)));
}

void DailymotionVideoWindow::onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status status) {
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
    
    if (video == m_video) {
        loadVideoUi();
    }
    
    hideProgressIndicator();
    disconnect(video, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
               this, SLOT(onVideoUpdateStatusChanged(QDailymotion::ResourcesRequest::Status)));
}
