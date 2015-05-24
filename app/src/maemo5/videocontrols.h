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

#ifndef VIDEOCONTROLS_H
#define VIDEOCONTROLS_H

#include "dailymotionstreammodel.h"
#include "pluginstreammodel.h"
#include "videomodel.h"
#include "vimeostreammodel.h"
#include "youtubestreammodel.h"
#include <QWidget>
#include <QMediaPlayer>

class Dialog;
class ImageCache;
class VideoPlayerButton;
class QSlider;
class QProgressBar;
class QLabel;
class QHBoxLayout;
class QTimer;
class QListView;
class QModelIndex;
class QMaemo5ListPickSelector;

class VideoControls : public QWidget
{
    Q_OBJECT

public:
    explicit VideoControls(QMediaPlayer *player, QWidget *parent = 0);
    ~VideoControls();
    
    void addVideo(Video *video);
    void addVideos(const QList<Video*> &videos);

    void keyPressEvent(QKeyEvent *e);

protected:
    void showEvent(QShowEvent *);
    void hideEvent(QHideEvent *);
    
private:
    void play(const Video *video);
    void play(const QUrl &url);
    
    void hideLoadingMessage();
    void showLoadingMessage();

private Q_SLOTS:
    void hideListView();
    void listViewUp();
    void listViewDown();
    
    void listStreams(bool showDialog = true);
    
    void next();
    void previous();
    
    void setCurrentIndex(const QModelIndex &index);
    
    void setPlaylistIndex();
    
    void setPosition();
    
    void setStream(const QModelIndex &index);
    
    void togglePlayPause();

    void onBufferStatusChanged(int progress);
    void onDurationChanged(qint64 duration);
    void onImageReady();
    void onMediaStatusChanged(QMediaPlayer::MediaStatus status);
    void onPositionChanged(qint64 position);
    void onStateChanged(QMediaPlayer::State state);
    
    void onDailymotionModelStatusChanged(QDailymotion::StreamsRequest::Status status);
    void onPluginModelStatusChanged(ResourcesRequest::Status status);
    void onVimeoModelStatusChanged(QVimeo::StreamsRequest::Status status);
    void onYouTubeModelStatusChanged(QYouTube::StreamsRequest::Status status);    

Q_SIGNALS:
    void back();
    
private:    
    QMediaPlayer *m_player;
    VideoModel *m_model;
    DailymotionStreamModel *m_dailymotionModel;
    PluginStreamModel *m_pluginModel;
    VimeoStreamModel *m_vimeoModel;
    YouTubeStreamModel *m_youtubeModel;
    ImageCache *m_cache;
    QTimer *m_listTimer;
    int m_currentIndex;
    qint64 m_resumePosition;
    
    QListView *m_view;
    QListView *m_streamView;
    Dialog *m_streamDialog;
    QMaemo5ListPickSelector *m_streamSelector;
    VideoPlayerButton *m_settingsButton;
    VideoPlayerButton *m_previousButton;
    VideoPlayerButton *m_playButton;
    VideoPlayerButton *m_nextButton;
    VideoPlayerButton *m_backButton;
    QSlider *m_progressSlider;
    QProgressBar *m_bufferProgressBar;
    QLabel *m_positionLabel;
    QLabel *m_durationLabel;
    QHBoxLayout *m_layout;
};

#endif // VIDEOCONTROLS_H
