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

#include "videocontrols.h"
#include "dialog.h"
#include "imagecache.h"
#include "resources.h"
#include "settings.h"
#include "utils.h"
#include "videoplaybackdelegate.h"
#include "videoplayerbutton.h"
#include <QSlider>
#include <QProgressBar>
#include <QLabel>
#include <QHBoxLayout>
#include <QTimer>
#include <QListView>
#include <QKeyEvent>
#include <QMaemo5InformationBox>

VideoControls::VideoControls(QMediaPlayer *player, QWidget *parent) :
    QWidget(parent),
    m_player(player),
    m_model(new VideoModel(this)),
    m_dailymotionModel(0),
    m_pluginModel(0),
    m_vimeoModel(0),
    m_youtubeModel(0),
    m_cache(new ImageCache),
    m_listTimer(new QTimer(this)),
    m_currentIndex(-1),
    m_resumePosition(0),
    m_view(new QListView(this)),
    m_streamView(0),
    m_streamDialog(0),
    m_settingsButton(new VideoPlayerButton(VideoPlayerButton::Settings, this)),
    m_previousButton(new VideoPlayerButton(VideoPlayerButton::Previous, this)),
    m_playButton(new VideoPlayerButton(VideoPlayerButton::PlayPause, this)),
    m_nextButton(new VideoPlayerButton(VideoPlayerButton::Next, this)),
    m_backButton(new VideoPlayerButton(VideoPlayerButton::Back, this)),
    m_progressSlider(new QSlider(Qt::Horizontal, this)),
    m_bufferProgressBar(new QProgressBar(this)),
    m_positionLabel(new QLabel("00:00", this)),
    m_durationLabel(new QLabel("00:00", this)),
    m_layout(new QHBoxLayout(this))
{
    setFixedSize(800, 70);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setStyleSheet("background:transparent");

    m_listTimer->setSingleShot(true);
    m_listTimer->setInterval(3000);

    m_progressSlider->setFocusPolicy(Qt::NoFocus);
    m_previousButton->setFocusPolicy(Qt::NoFocus);
    m_playButton->setFocusPolicy(Qt::NoFocus);
    m_nextButton->setFocusPolicy(Qt::NoFocus);
    m_backButton->setFocusPolicy(Qt::NoFocus);
    m_view->setFocusPolicy(Qt::NoFocus);

    m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_view->setVerticalScrollMode(QListView::ScrollPerItem);
    m_view->setModel(m_model);
    m_view->setItemDelegate(new VideoPlaybackDelegate(m_cache, m_view));

    m_settingsButton->setIcon(QIcon::fromTheme("camera_camera_setting"));
    m_playButton->setIcon(QIcon("/etc/hildon/theme/mediaplayer/Play.png"));
    m_backButton->setIcon(QIcon::fromTheme("general_backspace"));

    m_positionLabel->setFixedSize(70, 70);
    m_durationLabel->setFixedSize(70, 70);
    m_positionLabel->setAlignment(Qt::AlignCenter);
    m_durationLabel->setAlignment(Qt::AlignCenter);

    QFont font;
    font.setPixelSize(18);

    m_positionLabel->setFont(font);
    m_durationLabel->setFont(font);

    m_bufferProgressBar->setTextVisible(true);
    m_bufferProgressBar->setFormat(tr("Buffering"));
    m_bufferProgressBar->setMinimum(0);
    m_bufferProgressBar->setMaximum(100);

    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);
    m_layout->addWidget(m_positionLabel, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_progressSlider, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_bufferProgressBar, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_view, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_durationLabel, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_settingsButton, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_previousButton, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_playButton, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_nextButton, 0, Qt::AlignVCenter);
    m_layout->addWidget(m_backButton, 0, Qt::AlignVCenter);

    m_bufferProgressBar->hide();
    m_view->hide();

    connect(m_settingsButton, SIGNAL(clicked()), this, SLOT(listStreams()));
    connect(m_previousButton, SIGNAL(clicked()), this, SLOT(previous()));
    connect(m_playButton, SIGNAL(clicked()), this, SLOT(togglePlayPause()));
    connect(m_nextButton, SIGNAL(clicked()), this, SLOT(next()));
    connect(m_backButton, SIGNAL(clicked()), this, SIGNAL(back()));
    connect(m_progressSlider, SIGNAL(sliderReleased()), this, SLOT(setPosition()));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_player, SIGNAL(bufferStatusChanged(int)), this, SLOT(onBufferStatusChanged(int)));
    connect(m_player, SIGNAL(bufferStatusChanged(int)), m_bufferProgressBar, SLOT(setValue(int)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_listTimer, SIGNAL(timeout()), this, SLOT(hideListView()));
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(setCurrentIndex(QModelIndex)));
}

VideoControls::~VideoControls() {
    delete m_cache;
    m_cache = 0;
}

void VideoControls::addVideo(const Video *video) {
    m_model->append(video);
    
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia) {
        next();
    }
}

void VideoControls::addVideos(const QList<const Video*> &videos) {
    for (int i = 0; i < videos.size(); i++) {
        m_model->append(videos.at(i));
    }
    
    if (m_player->mediaStatus() == QMediaPlayer::NoMedia) {
        next();
    }
}

void VideoControls::keyPressEvent(QKeyEvent *e) {
    switch(e->key()) {
    case Qt::Key_S:
        listStreams();
        return;
    case Qt::Key_P:
    case Qt::Key_Space:
        togglePlayPause();
        return;
    case Qt::Key_Left:
        previous();
        return;
    case Qt::Key_Right:
        next();
        return;
    case Qt::Key_Up:
        listViewUp();
        return;
    case Qt::Key_Down:
        listViewDown();
        return;
    case Qt::Key_Return:
        setPlaylistIndex();
        return;
    case Qt::Key_Enter:
        setPlaylistIndex();
        return;
    }
}

void VideoControls::showEvent(QShowEvent *) {
    onPositionChanged(m_player->position());
    onDurationChanged(m_player->duration());

    connect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
    connect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
}

void VideoControls::hideEvent(QHideEvent *) {
    disconnect(m_player, SIGNAL(positionChanged(qint64)), this, SLOT(onPositionChanged(qint64)));
    disconnect(m_player, SIGNAL(durationChanged(qint64)), this, SLOT(onDurationChanged(qint64)));
}

void VideoControls::play(const Video *video) {
    if (!video->streamUrl().isEmpty()) {
        play(video->streamUrl());
    }
    else {
        listStreams(Settings::instance()->defaultPlaybackFormat(video->service()).isEmpty());
    }
}

void VideoControls::play(const QUrl &url) {
    m_player->setMedia(QMediaContent(url));
    m_player->play();
}

void VideoControls::hideLoadingMessage() {
    m_bufferProgressBar->setFormat(tr("Buffering"));
    m_progressSlider->show();
    m_bufferProgressBar->hide();
    m_view->hide();
    show();
}

void VideoControls::showLoadingMessage() {
    m_bufferProgressBar->setFormat(tr("Loading"));
    m_bufferProgressBar->show();
    m_progressSlider->hide();
    m_view->hide();
    show();
}

void VideoControls::hideListView() {
    m_view->hide();
    m_view->scrollTo(m_model->index(m_currentIndex));

    if (!m_bufferProgressBar->isVisible()) {
        m_progressSlider->show();
    }
}

void VideoControls::listViewUp() {
    if (!m_bufferProgressBar->isVisible()) {
        m_progressSlider->hide();
        m_view->show();
        m_view->scrollTo(m_view->indexAt(QPoint(0, -5)));
        m_listTimer->start();
        show();
    }
}

void VideoControls::listViewDown() {
    if (!m_bufferProgressBar->isVisible()) {
        m_progressSlider->hide();
        m_view->show();
        m_view->scrollTo(m_view->indexAt(QPoint(0, 75)));
        m_listTimer->start();
        show();
    }
}

void VideoControls::listStreams(bool showDialog) {
    const Video *video = m_model->get(m_currentIndex);
    
    if (!video) {
        return;
    }    
    
    if (showDialog) {
        m_player->pause();
        m_resumePosition = m_player->position();
        
        if (!m_streamDialog) {
            m_streamDialog = new Dialog(this);
            m_streamDialog->setWindowTitle(tr("Video format"));
            m_streamDialog->setAttribute(Qt::WA_DeleteOnClose, false);
            m_streamDialog->setFocusPolicy(Qt::NoFocus);
            m_streamView = new QListView(m_streamDialog);
            m_streamView->setFocusPolicy(Qt::NoFocus);
            m_streamView->setMinimumHeight(350);
            QVBoxLayout *vbox = new QVBoxLayout(m_streamDialog);
            vbox->addWidget(m_streamView);
            connect(m_streamView, SIGNAL(activated(QModelIndex)), m_streamDialog, SLOT(accept()));
            connect(m_streamView, SIGNAL(activated(QModelIndex)), this, SLOT(setStream(QModelIndex)));
        }
    }
    
    if (video->service() == Resources::YOUTUBE) {
        if (!m_youtubeModel) {
            m_youtubeModel = new YouTubeStreamModel(this);
            connect(m_youtubeModel, SIGNAL(statusChanged(QYouTube::StreamsRequest::Status)),
                    this, SLOT(onYouTubeModelStatusChanged(QYouTube::StreamsRequest::Status)));
        }
        
        if (showDialog) {
            m_streamView->setModel(m_youtubeModel);
            m_streamDialog->open();
            connect(m_streamDialog, SIGNAL(rejected()), m_youtubeModel, SLOT(cancel()), Qt::UniqueConnection);
        }
        
        m_youtubeModel->list(video->id());
    }
    else if (video->service() == Resources::DAILYMOTION) {
        if (!m_dailymotionModel) {
            m_dailymotionModel = new DailymotionStreamModel(this);
            connect(m_dailymotionModel, SIGNAL(statusChanged(QDailymotion::StreamsRequest::Status)),
                    this, SLOT(onDailymotionModelStatusChanged(QDailymotion::StreamsRequest::Status)));
        }
        
        if (showDialog) {
            m_streamView->setModel(m_dailymotionModel);
            m_streamDialog->open();
            connect(m_streamDialog, SIGNAL(rejected()), m_dailymotionModel, SLOT(cancel()), Qt::UniqueConnection);
        }
        
        m_dailymotionModel->list(video->id());
    }
    else if (video->service() == Resources::VIMEO) {
        if (!m_vimeoModel) {
            m_vimeoModel = new VimeoStreamModel(this);
            connect(m_vimeoModel, SIGNAL(statusChanged(QVimeo::StreamsRequest::Status)),
                    this, SLOT(onVimeoModelStatusChanged(QVimeo::StreamsRequest::Status)));
        }
        
        if (showDialog) {
            m_streamView->setModel(m_vimeoModel);
            m_streamDialog->open();
            connect(m_streamDialog, SIGNAL(rejected()), m_vimeoModel, SLOT(cancel()), Qt::UniqueConnection);
        }
        
        m_vimeoModel->list(video->id());
    }
    else {
        if (!m_pluginModel) {
            m_pluginModel = new PluginStreamModel(this);
            m_pluginModel->setService(video->service());
            connect(m_pluginModel, SIGNAL(statusChanged(ResourcesRequest::Status)),
                    this, SLOT(onPluginModelStatusChanged(ResourcesRequest::Status)));
        }
        
        if (showDialog) {
            m_streamView->setModel(m_pluginModel);
            m_streamDialog->open();
            connect(m_streamDialog, SIGNAL(rejected()), m_pluginModel, SLOT(cancel()), Qt::UniqueConnection);
        }
        
        m_pluginModel->list(video->id());
    }
}

void VideoControls::next() {
    if (m_currentIndex < m_model->rowCount() - 1) {
        m_currentIndex++;
        m_view->setCurrentIndex(m_model->index(m_currentIndex));
        
        if (const Video *video = m_model->get(m_currentIndex)) {
            play(video);
        }
    }
    else {
        emit back();
    }
}

void VideoControls::previous() {
    if (m_currentIndex > 0) {
        m_currentIndex--;

        if (m_currentIndex < m_model->rowCount()) {
            m_view->setCurrentIndex(m_model->index(m_currentIndex));
            
            if (const Video *video = m_model->get(m_currentIndex)) {
                play(video);
            }
        }
    }
    else {
        m_player->setPosition(0);
        m_player->play();
    }
}

void VideoControls::setCurrentIndex(const QModelIndex &index) {
    m_currentIndex = index.row() - 1;
    next();
}

void VideoControls::setPlaylistIndex() {
    if (m_view->isVisible()) {
        m_currentIndex = m_view->indexAt(QPoint(0, 0)).row() - 1;
        next();
    }
}

void VideoControls::setPosition() {
    qint64 pos = m_progressSlider->value() * 1000;
    m_player->setPosition(pos);
}

void VideoControls::setStream(const QModelIndex &index) {
    if (const Video *video = m_model->get(m_currentIndex)) {
        Settings::instance()->setDefaultPlaybackFormat(video->service(),
                                                       index.data(SelectionModel::NameRole).toString());
    }
    
    play(index.data(SelectionModel::ValueRole).toMap().value("url").toUrl());
}

void VideoControls::togglePlayPause() {
    switch (m_player->state()) {
    case QMediaPlayer::PlayingState:
        m_player->pause();
        return;
    default:
        m_player->play();
        return;
    }
}

void VideoControls::onBufferStatusChanged(int progress) {
    setVisible(progress < 100);
    m_bufferProgressBar->setVisible(progress < 100);
    m_progressSlider->setVisible(progress == 100);
    m_view->hide();
}

void VideoControls::onDurationChanged(qint64 duration) {
    m_durationLabel->setText(Utils::formatMSecs(duration));
    m_progressSlider->setMaximum(duration / 1000);
}


void VideoControls::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VideoControls::onMediaStatusChanged(QMediaPlayer::MediaStatus status) {
    switch (status) {
    case QMediaPlayer::BufferedMedia:
        if (m_resumePosition > 0) {
            m_player->setPosition(m_resumePosition);
            m_resumePosition = 0;
        }
        
        return;
    case QMediaPlayer::InvalidMedia:
        QMaemo5InformationBox::information(this, tr("Video cannot be played"), 0);
        return;
    case QMediaPlayer::EndOfMedia:
        next();
        return;
    default:
        return;
    }
}

void VideoControls::onPositionChanged(qint64 position) {
    m_positionLabel->setText(Utils::formatMSecs(position));

    if (!m_progressSlider->isSliderDown()) {
        m_progressSlider->setValue(position / 1000);
    }
}

void VideoControls::onStateChanged(QMediaPlayer::State state) {
    switch (state) {
    case QMediaPlayer::PlayingState:
        m_playButton->setIcon(QIcon("/etc/hildon/theme/mediaplayer/Pause.png"));
        return;
    default:
        m_playButton->setIcon(QIcon("/etc/hildon/theme/mediaplayer/Play.png"));
        return;
    }
}

void VideoControls::onDailymotionModelStatusChanged(QDailymotion::StreamsRequest::Status status) {
    switch (status) {
    case QDailymotion::StreamsRequest::Loading:
        showLoadingMessage();
        
        if (m_streamDialog) {
            m_streamDialog->showProgressIndicator();
        }

        return;
    case QDailymotion::StreamsRequest::Ready:
        hideLoadingMessage();

        if (m_streamDialog) {
            m_streamDialog->hideProgressIndicator();
        }
        
        if (m_dailymotionModel->rowCount() > 0) {
            const int i = m_dailymotionModel->match("name",
                                                    Settings::instance()->defaultPlaybackFormat(Resources::DAILYMOTION));
            
            if (i >= 0) {
                if ((m_streamView) && (m_streamView->isVisible())) {
                    m_streamView->setCurrentIndex(m_dailymotionModel->index(i));
                }
                else {
                    play(m_dailymotionModel->data(i, "value").toMap().value("url").toUrl());
                }
                
                if (m_streamDialog) {
                    m_streamDialog->hideProgressIndicator();
                }
            }
            
            if (m_streamDialog) {
                m_streamDialog->hideProgressIndicator();
            }
            
            return;
        }
        
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        return;
    case QDailymotion::StreamsRequest::Canceled:
        m_resumePosition = 0;
        break;
    case QDailymotion::StreamsRequest::Failed:
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        break;
    default:
        break;
    }
    
    hideLoadingMessage();
    
    if (m_streamDialog) {
        m_streamDialog->hideProgressIndicator();
    }
}

void VideoControls::onPluginModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showLoadingMessage();
        
        if (m_streamDialog) {
            m_streamDialog->showProgressIndicator();
        }

        return;
    case ResourcesRequest::Ready:
        hideLoadingMessage();

        if (m_streamDialog) {
            m_streamDialog->hideProgressIndicator();
        }
        
        if (m_pluginModel->rowCount() > 0) {            
            const int i = m_pluginModel->match("name",
                                               Settings::instance()->defaultPlaybackFormat(m_pluginModel->service()));
            
            if (i >= 0) {
                if ((m_streamView) && (m_streamView->isVisible())) {
                    m_streamView->setCurrentIndex(m_pluginModel->index(i));
                }
                else {
                    play(m_pluginModel->data(i, "value").toMap().value("url").toUrl());
                }
            }
            
            return;
        }
        
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        return;
    case ResourcesRequest::Canceled:
        m_resumePosition = 0;
        break;
    case ResourcesRequest::Failed:
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        break;
    default:
        break;
    }
    
    hideLoadingMessage();
    
    if (m_streamDialog) {
        m_streamDialog->hideProgressIndicator();
    }
}

void VideoControls::onVimeoModelStatusChanged(QVimeo::StreamsRequest::Status status) {
    switch (status) {
    case QVimeo::StreamsRequest::Loading:
        showLoadingMessage();
        
        if (m_streamDialog) {
            m_streamDialog->showProgressIndicator();
        }

        return;
    case QVimeo::StreamsRequest::Ready:
        hideLoadingMessage();

        if (m_streamDialog) {
            m_streamDialog->hideProgressIndicator();
        }
        
        if (m_vimeoModel->rowCount() > 0) {
            const int i = m_vimeoModel->match("name",
                                              Settings::instance()->defaultPlaybackFormat(Resources::VIMEO));
            
            if (i >= 0) {
                if ((m_streamView) && (m_streamView->isVisible())) {
                    m_streamView->setCurrentIndex(m_vimeoModel->index(i));
                }
                else {
                    play(m_vimeoModel->data(i, "value").toMap().value("url").toUrl());
                }
            }
            
            return;
        }
        
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        return;
    case QVimeo::StreamsRequest::Canceled:
        m_resumePosition = 0;
        break;
    case QVimeo::StreamsRequest::Failed:
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        break;
    default:
        break;
    }
    
    hideLoadingMessage();
    
    if (m_streamDialog) {
        m_streamDialog->hideProgressIndicator();
    }
}

void VideoControls::onYouTubeModelStatusChanged(QYouTube::StreamsRequest::Status status) {
    switch (status) {
    case QYouTube::StreamsRequest::Loading:
        showLoadingMessage();
        
        if (m_streamDialog) {
            m_streamDialog->showProgressIndicator();
        }
        
        return;
    case QYouTube::StreamsRequest::Ready:
        hideLoadingMessage();

        if (m_streamDialog) {
            m_streamDialog->hideProgressIndicator();
        }
        
        if (m_youtubeModel->rowCount() > 0) {
            const int i = m_youtubeModel->match("name",
                                                Settings::instance()->defaultPlaybackFormat(Resources::YOUTUBE));
            
            if (i >= 0) {
                if ((m_streamView) && (m_streamView->isVisible())) {
                    m_streamView->setCurrentIndex(m_youtubeModel->index(i));
                }
                else {
                    play(m_youtubeModel->data(i, "value").toMap().value("url").toUrl());
                }
            }
            
            return;
        }
        
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        return;
    case QYouTube::StreamsRequest::Canceled:
        m_resumePosition = 0;
        break;
    case QYouTube::StreamsRequest::Failed:
        QMaemo5InformationBox::information(this, tr("No streams found"), 0);
        break;
    default:
        break;
    }
    
    hideLoadingMessage();
    
    if (m_streamDialog) {
        m_streamDialog->hideProgressIndicator();
    }
}
