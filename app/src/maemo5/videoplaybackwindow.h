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

#ifndef VIDEOPLAYBACKWINDOW_H
#define VIDEOPLAYBACKWINDOW_H

#include "stackedwindow.h"
#include "videowidget.h"
#include "videocontrols.h"

class VideoPlaybackWindow : public StackedWindow
{
    Q_OBJECT

public:
    explicit VideoPlaybackWindow(StackedWindow *parent = 0);
    ~VideoPlaybackWindow();
    
    void addVideo(CTVideo *video);
    void addVideos(const QList<CTVideo*> &videos);

protected:
    bool event(QEvent *e);
    void keyPressEvent(QKeyEvent *e);

private Q_SLOTS:
    void toggleControls();
    
    void onStateChanged(QMediaPlayer::State state);
    
private:
    VideoWidget *m_videoWidget;
    QMediaPlayer *m_player;
    VideoControls *m_controls;
};

#endif // VIDEOPLAYBACKWINDOW_H
