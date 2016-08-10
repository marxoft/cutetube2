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

#ifndef VIDEOPLAYERWINDOW_H
#define VIDEOPLAYERWINDOW_H

#include "dailymotionstreammodel.h"
#include "pluginstreammodel.h"
#include "videomodel.h"
#include "videoplayer.h"
#include "vimeostreammodel.h"
#include "youtubestreammodel.h"
#include <QMainWindow>

class QComboBox;
class QTextEdit;

class VideoPlayerWindow : public QMainWindow
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)

public:
    ~VideoPlayerWindow();
    
    static VideoPlayerWindow* instance();
    
    int count() const;
    
    int currentIndex() const;
    
public Q_SLOTS:    
    void append(CTVideo *video);
    void append(const QList<CTVideo*> &videos);
    void insert(int index, CTVideo *video);
    void insert(int index, const QList<CTVideo*> &videos);
    bool remove(int index);
    
    void play();
    void play(CTVideo *video);
    void stop();
    
    void next();
    void previous();
    void setCurrentIndex(int index);

private Q_SLOTS:    
    void onFormatChanged(int index);
    
    void onPlayerReadyRead();
    
    void onDailymotionModelStatusChanged(QDailymotion::StreamsRequest::Status status);
    void onPluginModelStatusChanged(ResourcesRequest::Status status);
    void onVimeoModelStatusChanged(QVimeo::StreamsRequest::Status status);
    void onYouTubeModelStatusChanged(QYouTube::StreamsRequest::Status status);

Q_SIGNALS:
    void countChanged(int count);
    void currentIndexChanged(int index);

private:
    VideoPlayerWindow();
    
    virtual void closeEvent(QCloseEvent *event);
    
    CTVideo* get(int index) const;
    
    DailymotionStreamModel* dailymotionModel();
    PluginStreamModel* pluginModel();
    VimeoStreamModel* vimeoModel();
    YouTubeStreamModel *youtubeModel();
    
    static VideoPlayerWindow *self;
    
    VideoPlayer *m_player;
    
    DailymotionStreamModel *m_dailymotionModel;
    PluginStreamModel *m_pluginModel;
    VimeoStreamModel *m_vimeoModel;
    YouTubeStreamModel *m_youtubeModel;
    
    VideoModel *m_playlistModel;
    
    QMenu *m_playbackMenu;
    
    QToolBar *m_toolBar;
        
    QComboBox *m_formatSelector;
    
    QTextEdit *m_outputEdit;
    
    QAction *m_playAction;
    QAction *m_stopAction;
    QAction *m_nextAction;
    QAction *m_previousAction;
    
    int m_index;    
};

#endif // VIDEOPLAYERWINDOW_H
