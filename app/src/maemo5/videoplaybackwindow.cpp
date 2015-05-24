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

#include "videoplaybackwindow.h"
#include "screensaver.h"
#include <QVBoxLayout>
#include <QKeyEvent>

VideoPlaybackWindow::VideoPlaybackWindow(StackedWindow *parent) :
    StackedWindow(parent),
    m_videoWidget(new VideoWidget(this)),
    m_player(new QMediaPlayer(this)),
    m_controls(new VideoControls(m_player, this))
{
    setWindowState(Qt::WindowFullScreen);
    setWindowTitle(tr("Now playing"));
    setCentralWidget(new QWidget);

    QVBoxLayout *vbox = new QVBoxLayout(centralWidget());
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->setSpacing(0);
    vbox->addWidget(m_videoWidget);
    vbox->addWidget(m_controls);

    preventScreenBlanking(winId(), true);

    m_player->setVideoOutput(m_videoWidget);
    m_player->setVolume(100);

    m_controls->hide();

    connect(m_videoWidget, SIGNAL(clicked()), this, SLOT(toggleControls()));
    connect(m_controls, SIGNAL(back()), this, SLOT(close()));
    connect(m_player, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(onStateChanged(QMediaPlayer::State)));
}

VideoPlaybackWindow::~VideoPlaybackWindow() {
    m_player->stop();
    preventScreenBlanking(winId(), false);
}

void VideoPlaybackWindow::addVideo(Video *video) {
    m_controls->addVideo(video);
}

void VideoPlaybackWindow::addVideos(const QList<Video*> &videos) {
    m_controls->addVideos(videos);
}

bool VideoPlaybackWindow::event(QEvent *e) {
    switch (e->type()) {
    case QEvent::WindowActivate:
        preventScreenBlanking(winId(), m_player->state() == QMediaPlayer::PlayingState);
        break;
    case QEvent::WindowDeactivate:
        preventScreenBlanking(winId(), false);
        break;
    default:
        break;
    }

    return StackedWindow::event(e);
}

void VideoPlaybackWindow::keyPressEvent(QKeyEvent *e) {
    switch (e->key()) {
    case Qt::Key_F:
        toggleControls();
        return;
    case Qt::Key_Backspace:
        close();
        return;
    default:
        m_controls->keyPressEvent(e);
        return;
    }
}

void VideoPlaybackWindow::toggleControls() {
    m_controls->setVisible(!m_controls->isVisible());
}

void VideoPlaybackWindow::onStateChanged(QMediaPlayer::State state) {
    switch (state) {
    case QMediaPlayer::PlayingState:
        preventScreenBlanking(winId(), isActiveWindow());
        return;
    default:
        preventScreenBlanking(winId(), false);
        return;
    }
}
