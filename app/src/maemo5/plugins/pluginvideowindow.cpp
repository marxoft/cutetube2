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

#include "pluginvideowindow.h"
#include "clipboard.h"
#include "commentdelegate.h"
#include "image.h"
#include "imagecache.h"
#include "listview.h"
#include "plugincommentmodel.h"
#include "plugindownloaddialog.h"
#include "pluginplaybackdialog.h"
#include "pluginplaylistwindow.h"
#include "pluginuser.h"
#include "pluginuserwindow.h"
#include "resources.h"
#include "resourcesplugins.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include "videodelegate.h"
#include "videolauncher.h"
#include "videoplaybackwindow.h"
#include "videothumbnail.h"
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

PluginVideoWindow::PluginVideoWindow(const QString &service, const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new PluginVideo(this)),
    m_user(new PluginUser(this)),
    m_relatedModel(new PluginVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new VideoDelegate(m_cache, PluginVideoModel::DateRole, PluginVideoModel::DurationRole,
                                        PluginVideoModel::ThumbnailUrlRole, PluginVideoModel::TitleRole,
                                        PluginVideoModel::UsernameRole, this)),
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
    m_downloadAction(0),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_contextMenu(new QMenu(this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    connect(m_video, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onVideoStatusChanged(ResourcesRequest::Status)));
            
    m_video->loadVideo(service, id);
}

PluginVideoWindow::PluginVideoWindow(const PluginVideo *video, StackedWindow *parent) :
    StackedWindow(parent),
    m_video(new PluginVideo(video, this)),
    m_user(new PluginUser(this)),
    m_relatedModel(new PluginVideoModel(this)),
    m_commentModel(0),
    m_cache(new ImageCache),
    m_thumbnail(new VideoThumbnail(this)),
    m_avatar(new Image(this)),
    m_relatedView(new ListView(this)),
    m_commentView(0),
    m_relatedDelegate(new VideoDelegate(m_cache, PluginVideoModel::DateRole, PluginVideoModel::DurationRole,
                                        PluginVideoModel::ThumbnailUrlRole, PluginVideoModel::TitleRole,
                                        PluginVideoModel::UsernameRole, this)),
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
    m_downloadAction(0),
    m_shareAction(new QAction(tr("Copy URL"), this)),
    m_contextMenu(new QMenu(this)),
    m_relatedDownloadAction(new QAction(tr("Download"), this)),
    m_relatedShareAction(new QAction(tr("Copy URL"), this))
{
    loadBaseUi();
    loadVideoUi();
    getRelatedVideos();
    
    if (!video->userId().isEmpty()) {
        connect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                SLOT(onUserStatusChanged(ResourcesRequest::Status)));
            
        m_user->loadUser(video->service(), video->userId());
    }
}

PluginVideoWindow::~PluginVideoWindow() {
    delete m_cache;
    m_cache = 0;
}

void PluginVideoWindow::loadBaseUi() {
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
    menuBar()->addAction(m_shareAction);
    
    connect(m_relatedModel, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onRelatedModelStatusChanged(ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_relatedView, SIGNAL(activated(QModelIndex)), this, SLOT(showRelatedVideo(QModelIndex)));
    connect(m_relatedView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
    connect(m_relatedDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playRelatedVideo(QModelIndex)));
    connect(m_tabBar, SIGNAL(currentChanged(int)), this, SLOT(onTabIndexChanged(int)));
    connect(m_listAction, SIGNAL(triggered()), this, SLOT(enableListMode()));
    connect(m_gridAction, SIGNAL(triggered()), this, SLOT(enableGridMode()));
    connect(m_reloadAction, SIGNAL(triggered()), this, SLOT(reload()));
    connect(m_thumbnail, SIGNAL(clicked()), this, SLOT(playVideo()));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_shareAction, SIGNAL(triggered()), this, SLOT(shareVideo()));
    connect(m_relatedDownloadAction, SIGNAL(triggered()), this, SLOT(downloadRelatedVideo()));
    connect(m_relatedShareAction, SIGNAL(triggered()), this, SLOT(shareRelatedVideo()));
    
    if (m_video->isDownloadable()) {
        m_downloadAction = new QAction(tr("Download"), this);
        menuBar()->insertAction(m_shareAction, m_downloadAction);
        connect(m_downloadAction, SIGNAL(triggered()), this, SLOT(downloadVideo()));
    }
        
    if (Settings::instance()->defaultViewMode() == "grid") {
        m_gridAction->trigger();
    }
}

void PluginVideoWindow::loadUserUi() {
    m_userLabel->setText(m_userLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_avatar->setSource(m_user->thumbnailUrl());
    connect(m_avatar, SIGNAL(clicked()), this, SLOT(showUser()));
}

void PluginVideoWindow::loadVideoUi() {
    m_titleLabel->setText(m_video->title());
    m_dateLabel->setText(tr("Published on %1").arg(m_video->date()));
    m_descriptionLabel->setHtml(Utils::toRichText(m_video->description()));
    m_thumbnail->setSource(m_video->largeThumbnailUrl());
    m_thumbnail->setDuration(m_video->duration());
    
    if (ResourcesPlugins::instance()->resourceTypeIsSupported(m_video->service(), Resources::COMMENT)) {
        m_tabBar->addTab(tr("Comments"));
    }
}

void PluginVideoWindow::enableGridMode() {
    m_relatedDelegate->setGridMode(true);
    m_relatedView->setFlow(QListView::LeftToRight);
    m_relatedView->setWrapping(true);
    Settings::instance()->setDefaultViewMode("grid");
}

void PluginVideoWindow::enableListMode() {
    m_relatedDelegate->setGridMode(false);
    m_relatedView->setFlow(QListView::TopToBottom);
    m_relatedView->setWrapping(false);
    Settings::instance()->setDefaultViewMode("list");
}

void PluginVideoWindow::getRelatedVideos() {
    m_relatedModel->setService(m_video->service());
    m_relatedModel->list(m_video->id());
}

void PluginVideoWindow::downloadVideo() {
    if (isBusy()) {
        return;
    }
    
    PluginDownloadDialog *dialog = new PluginDownloadDialog(m_video->service(), m_video->id(), m_video->streamUrl(),
                                                            m_video->title(), this);
    dialog->open();
}

void PluginVideoWindow::playVideo() {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        VideoPlaybackWindow *window = new VideoPlaybackWindow(this);
        window->show();
        window->addVideo(m_video);
    }
    else if (!m_video->streamUrl().isEmpty()) {
        VideoLauncher::playVideo(m_video->streamUrl().toString());
    }
    else {
        PluginPlaybackDialog *dialog = new PluginPlaybackDialog(m_video->service(), m_video->id(), m_video->title(), this);
        dialog->open();
    }
}

void PluginVideoWindow::shareVideo() {
    Clipboard::instance()->setText(m_video->url().toString());
    QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
}

void PluginVideoWindow::downloadRelatedVideo() {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        QString id = m_relatedView->currentIndex().data(PluginVideoModel::IdRole).toString();
        QString title = m_relatedView->currentIndex().data(PluginVideoModel::TitleRole).toString();
        QUrl streamUrl = m_relatedView->currentIndex().data(PluginVideoModel::StreamUrlRole).toString();
        
        PluginDownloadDialog *dialog = new PluginDownloadDialog(m_video->service(), id, streamUrl, title, this);
        dialog->open();
    }
}

void PluginVideoWindow::playRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (Settings::instance()->videoPlayer() == "cutetube") {
        if (PluginVideo *video = m_relatedModel->get(index.row())) {
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
    
            PluginPlaybackDialog *dialog = new PluginPlaybackDialog(m_video->service(), id, title, this);
            dialog->open();
        }
    }
}

void PluginVideoWindow::shareRelatedVideo() {
    if (const PluginVideo *video = m_relatedModel->get(m_relatedView->currentIndex().row())) {
        Clipboard::instance()->setText(video->url().toString());
        QMaemo5InformationBox::information(this, tr("URL copied to clipboard"));
    }
}

void PluginVideoWindow::showRelatedVideo(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    if (PluginVideo *video = m_relatedModel->get(index.row())) {
        PluginVideoWindow *window = new PluginVideoWindow(video, this);
        window->show();
    }
}

void PluginVideoWindow::reload() {
    if (m_tabBar->currentIndex() == 1) {
        if (m_commentModel) {
            m_commentModel->reload();
        }
    }
    else {
        m_relatedModel->reload();
    }
}

void PluginVideoWindow::showContextMenu(const QPoint &pos) {
    if ((!isBusy()) && (m_relatedView->currentIndex().isValid())) {
        m_relatedDownloadAction->setEnabled(m_relatedModel->data(m_relatedView->currentIndex(),
                                                                 PluginVideoModel::DownloadableRole).toBool());
        m_contextMenu->popup(pos, m_relatedDownloadAction);
    }
}

void PluginVideoWindow::showComments() {
    if (!m_commentView) {
        m_commentView = new ListView(this);
        m_commentDelegate = new CommentDelegate(m_cache, PluginCommentModel::BodyRole, PluginCommentModel::DateRole,
                                                PluginCommentModel::ThumbnailUrlRole, PluginCommentModel::UsernameRole,
                                                this);
        m_commentModel = new PluginCommentModel(this);
        m_commentModel->setService(m_video->service());
        m_commentView->setUniformItemSizes(false);
        m_commentView->setModel(m_commentModel);
        m_commentView->setItemDelegate(m_commentDelegate);
        m_noCommentsLabel = new QLabel(QString("<p align='center'; style='font-size: 40px; color: %1'>%2</p>")
                                      .arg(palette().color(QPalette::Mid).name()).arg(tr("No comments found")), this);
        m_stack->addWidget(m_commentView);
        m_stack->addWidget(m_noCommentsLabel);
        
        connect(m_commentDelegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(showUser(QModelIndex)));
        connect(m_commentModel, SIGNAL(statusChanged(ResourcesRequest::Status)),
                this, SLOT(onCommentModelStatusChanged(ResourcesRequest::Status)));
                
        m_commentModel->list(m_video->id());
    }
    
    if ((m_commentModel->rowCount() == 0) && (m_commentModel->status() != ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noCommentsLabel);
    }
    else {
        m_stack->setCurrentWidget(m_commentView);
    }
}

void PluginVideoWindow::showRelatedVideos() {
    if ((m_relatedModel->rowCount() == 0) && (m_relatedModel->status() != ResourcesRequest::Loading)) {
        m_stack->setCurrentWidget(m_noVideosLabel);
    }
    else {
        m_stack->setCurrentWidget(m_relatedView);
    }
}

void PluginVideoWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != m_video->service()) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        PluginPlaylistWindow *window = new PluginPlaylistWindow(m_video->service(), resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        PluginUserWindow *window = new PluginUserWindow(m_video->service(), resource.value("id").toString(), this);
        window->show();
    }
    else {
        PluginVideoWindow *window = new PluginVideoWindow(m_video->service(), resource.value("id").toString(), this);
        window->show();
    }
}

void PluginVideoWindow::showUser() {
    if (isBusy()) {
        return;
    }
    
    PluginUserWindow *window = new PluginUserWindow(m_user, this);
    window->show();
}

void PluginVideoWindow::showUser(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    PluginUserWindow *window = new PluginUserWindow(m_video->service(),
                                                    index.data(PluginCommentModel::UserIdRole).toString(), this);
    window->show();
}

void PluginVideoWindow::onImageReady() {
    if ((m_commentView) && (m_commentView->isVisible())) {
        m_commentView->viewport()->update(m_commentView->viewport()->rect());
    }
    else {
        m_relatedView->viewport()->update(m_relatedView->viewport()->rect());
    }
}

void PluginVideoWindow::onCommentModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        showComments();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
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

void PluginVideoWindow::onRelatedModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        showRelatedVideos();
        m_reloadAction->setEnabled(false);
        return;
    case ResourcesRequest::Failed:
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

void PluginVideoWindow::onTabIndexChanged(int index) {
    if (index == 1) {
        showComments();
    }
    else {
        showRelatedVideos();
    }
}

void PluginVideoWindow::onUserStatusChanged(ResourcesRequest::Status status) {
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

void PluginVideoWindow::onVideoStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        loadVideoUi();
        getRelatedVideos();
        
        if (!m_video->userId().isEmpty()) {
            connect(m_user, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
                    SLOT(onUserStatusChanged(ResourcesRequest::Status)));
            m_user->loadUser(m_video->service(), m_video->userId());
        }
        
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_video->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_video, SIGNAL(statusChanged(ResourcesRequest::Status)),
               this, SLOT(onVideoStatusChanged(ResourcesRequest::Status)));
}
