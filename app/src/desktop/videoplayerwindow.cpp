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

#include "videoplayerwindow.h"
#include "resources.h"
#include "settings.h"
#include <QComboBox>
#include <QMenu>
#include <QMenuBar>
#include <QTextEdit>
#include <QToolBar>

VideoPlayerWindow* VideoPlayerWindow::self = 0;

VideoPlayerWindow::VideoPlayerWindow() :
    QMainWindow(),
    m_player(new VideoPlayer(this)),
    m_dailymotionModel(0),
    m_pluginModel(0),
    m_vimeoModel(0),
    m_youtubeModel(0),
    m_playlistModel(new VideoModel(this)),
    m_playbackMenu(new QMenu(tr("&Playback"), this)),
    m_toolBar(new QToolBar(this)),
    m_formatSelector(new QComboBox(this)),
    m_outputEdit(new QTextEdit(this)),
    m_playAction(new QAction(QIcon::fromTheme("media-playback-start"), tr("&Play"), this)),
    m_stopAction(new QAction(QIcon::fromTheme("media-playback-stop"), tr("&Stop"), this)),
    m_nextAction(new QAction(QIcon::fromTheme("media-skip-forward"), tr("&Next"), this)),
    m_previousAction(new QAction(QIcon::fromTheme("media-skip-backward"), tr("&Previous"), this)),
    m_index(-1)
{
    setWindowTitle(tr("Video player"));
    addToolBar(Qt::TopToolBarArea, m_toolBar);
    
    statusBar();
    
    menuBar()->addMenu(m_playbackMenu);
    
    setCentralWidget(m_outputEdit);
    
    m_playbackMenu->addAction(m_playAction);
    m_playbackMenu->addAction(m_stopAction);
    
    m_toolBar->setObjectName("videoPlayerToolBar");
    m_toolBar->setWindowTitle(tr("Main toolbar"));
    m_toolBar->setAllowedAreas(Qt::TopToolBarArea);
    m_toolBar->setMovable(false);
    m_toolBar->addAction(m_playAction);
    m_toolBar->addAction(m_stopAction);
    m_toolBar->addAction(m_previousAction);
    m_toolBar->addAction(m_nextAction);
    m_toolBar->addWidget(m_formatSelector);
    
    connect(m_player, SIGNAL(readyReadOutput()), this, SLOT(onPlayerReadyRead()));
    connect(m_formatSelector, SIGNAL(activated(int)), this, SLOT(onFormatChanged(int)));
    connect(m_playAction, SIGNAL(triggered()), this, SLOT(play()));
    connect(m_stopAction, SIGNAL(triggered()), this, SLOT(stop()));
    connect(m_nextAction, SIGNAL(triggered()), this, SLOT(next()));
    connect(m_previousAction, SIGNAL(triggered()), this, SLOT(previous()));
    
    restoreGeometry(Settings::videoPlayerWindowGeometry());
    restoreState(Settings::videoPlayerWindowState());
}

VideoPlayerWindow::~VideoPlayerWindow() {
    self = 0;
}

void VideoPlayerWindow::closeEvent(QCloseEvent *event) {
    Settings::setVideoPlayerWindowGeometry(saveGeometry());
    Settings::setVideoPlayerWindowState(saveState());
    QMainWindow::closeEvent(event);
}

VideoPlayerWindow* VideoPlayerWindow::instance() {
    return self ? self : self = new VideoPlayerWindow;
}

int VideoPlayerWindow::count() const {
    return m_playlistModel->rowCount();
}

int VideoPlayerWindow::currentIndex() const {
    return m_index;
}

void VideoPlayerWindow::setCurrentIndex(int index) {
    if ((index != currentIndex()) && (index >= -1) && (index < count())) {
        if (index == -1) {
            stop();
        }
        
        m_index = index;
        emit currentIndexChanged(index);
    }
}

void VideoPlayerWindow::next() {
    if (currentIndex() < count() - 1) {
        setCurrentIndex(currentIndex() + 1);
        stop();
        play();
    }
}

void VideoPlayerWindow::previous() {
    if (currentIndex() > 0) {
        setCurrentIndex(currentIndex() - 1);
        stop();
        play();
    }
}

void VideoPlayerWindow::append(CTVideo *video) {
    m_playlistModel->append(video);
    emit countChanged(count());
}

void VideoPlayerWindow::append(const QList<CTVideo*> &videos) {
    foreach (CTVideo *video, videos) {
        m_playlistModel->append(video);
    }
    
    emit countChanged(count());
}

void VideoPlayerWindow::insert(int index, CTVideo *video) {
    m_playlistModel->insert(index, video);
    emit countChanged(count());
}

void VideoPlayerWindow::insert(int index, const QList<CTVideo*> &videos) {
    foreach (CTVideo *video, videos) {
        m_playlistModel->insert(index++, video);
    }
    
    emit countChanged(count());
}

bool VideoPlayerWindow::remove(int index) {
    if ((index >= 0) && (index < count())) {
        if (index == currentIndex()) {
            stop();
        }
        
        m_playlistModel->remove(index);
        
        if (currentIndex() > count()) {
            setCurrentIndex(count() - 1);
        }
        else {
            setCurrentIndex(currentIndex() + 1);
        }
        
        return true;
    }
    
    return false;
}

void VideoPlayerWindow::play() {
    if (const CTVideo *video = m_playlistModel->get(currentIndex())) {
        const QString service = video->service();
        
        if (service == Resources::DAILYMOTION) {
            m_formatSelector->setModel(dailymotionModel());
            dailymotionModel()->list(video->id());
        }
        else if (service == Resources::VIMEO) {
            m_formatSelector->setModel(vimeoModel());
            vimeoModel()->list(video->id());
        }
        else if (service == Resources::YOUTUBE) {
            m_formatSelector->setModel(youtubeModel());
            youtubeModel()->list(video->id());
        }
        else {
            m_formatSelector->setModel(pluginModel());
            pluginModel()->setService(service);
            pluginModel()->clear();
            
            const QUrl url = video->streamUrl();
            
            if (!url.isEmpty()) {
                pluginModel()->append(tr("Default"), url);
                m_player->play(url.toString());
            }
            else {
                pluginModel()->list(video->id());
            }
        }
    }
}

void VideoPlayerWindow::play(CTVideo *video) {
    append(video);
    next();
}

void VideoPlayerWindow::stop() {
    m_player->stop();
}

DailymotionStreamModel* VideoPlayerWindow::dailymotionModel() {
    if (!m_dailymotionModel) {
        m_dailymotionModel = new DailymotionStreamModel(this);
        connect(m_dailymotionModel, SIGNAL(statusChanged(QDailymotion::StreamsRequest::Status)),
                this, SLOT(onDailymotionModelStatusChanged(QDailymotion::StreamsRequest::Status)));
    }
    
    return m_dailymotionModel;
}

PluginStreamModel* VideoPlayerWindow::pluginModel() {
    if (!m_pluginModel) {
        m_pluginModel = new PluginStreamModel(this);
        connect(m_pluginModel, SIGNAL(statusChanged(ResourcesRequest::Status)),
                this, SLOT(onPluginModelStatusChanged(ResourcesRequest::Status)));
    }
    
    return m_pluginModel;
}

VimeoStreamModel* VideoPlayerWindow::vimeoModel() {
    if (!m_vimeoModel) {
        m_vimeoModel = new VimeoStreamModel(this);
        connect(m_vimeoModel, SIGNAL(statusChanged(QVimeo::StreamsRequest::Status)),
                this, SLOT(onVimeoModelStatusChanged(QVimeo::StreamsRequest::Status)));
    }
    
    return m_vimeoModel;
}

YouTubeStreamModel* VideoPlayerWindow::youtubeModel() {
    if (!m_youtubeModel) {
        m_youtubeModel= new YouTubeStreamModel(this);
        connect(m_youtubeModel, SIGNAL(statusChanged(QYouTube::StreamsRequest::Status)),
                this, SLOT(onYouTubeModelStatusChanged(QYouTube::StreamsRequest::Status)));
    }
    
    return m_youtubeModel;
}

void VideoPlayerWindow::onFormatChanged(int index) {
    if (const CTVideo *video = m_playlistModel->get(currentIndex())) {
        Settings::setDefaultPlaybackFormat(video->service(), m_formatSelector->currentText());
    }
    
    m_player->play(m_formatSelector->itemData(index).toMap().value("url").toString());
}

void VideoPlayerWindow::onPlayerReadyRead() {
    m_outputEdit->append(m_player->readAllOutput());
}

void VideoPlayerWindow::onDailymotionModelStatusChanged(QDailymotion::StreamsRequest::Status status) {
    switch (status) {
    case QDailymotion::StreamsRequest::Ready:
        m_formatSelector->setCurrentIndex(qMax(0, dailymotionModel()->match("name",
                                          Settings::defaultPlaybackFormat(Resources::DAILYMOTION))));
        m_player->play(m_formatSelector->itemData(m_formatSelector->currentIndex()).toMap().value("url").toString());
        break;
    default:
        break;
    }
}

void VideoPlayerWindow::onPluginModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Ready:
        m_formatSelector->setCurrentIndex(qMax(0, pluginModel()->match("name",
                                          Settings::defaultPlaybackFormat(pluginModel()->service()))));
        m_player->play(m_formatSelector->itemData(m_formatSelector->currentIndex()).toMap().value("url").toString());
        break;
    default:
        break;
    }
}

void VideoPlayerWindow::onVimeoModelStatusChanged(QVimeo::StreamsRequest::Status status) {
    switch (status) {
    case QVimeo::StreamsRequest::Ready:
        m_formatSelector->setCurrentIndex(qMax(0, vimeoModel()->match("name",
                                          Settings::defaultPlaybackFormat(Resources::VIMEO))));
        m_player->play(m_formatSelector->itemData(m_formatSelector->currentIndex()).toMap().value("url").toString());
        break;
    default:
        break;
    }
}

void VideoPlayerWindow::onYouTubeModelStatusChanged(QYouTube::StreamsRequest::Status status) {
    switch (status) {
    case QYouTube::StreamsRequest::Ready:
        m_formatSelector->setCurrentIndex(qMax(0, youtubeModel()->match("name",
                                          Settings::defaultPlaybackFormat(Resources::YOUTUBE))));
        m_player->play(m_formatSelector->itemData(m_formatSelector->currentIndex()).toMap().value("url").toString());
        break;
    default:
        break;
    }
}
