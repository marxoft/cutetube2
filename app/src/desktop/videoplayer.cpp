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

#include "videoplayer.h"
#include "logger.h"
#include "settings.h"
#include <QProcess>

VideoPlayer::VideoPlayer(QObject *parent) :
    QObject(parent),
    m_process(0),
    m_status(Null)
{
}

QString VideoPlayer::errorString() const {
    return m_errorString;
}

void VideoPlayer::setErrorString(const QString &e) {
    m_errorString = e;
}

VideoPlayer::Status VideoPlayer::status() const {
    return m_status;
}

void VideoPlayer::setStatus(VideoPlayer::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString VideoPlayer::readAllOutput() const {
    return m_process ? QString::fromUtf8(m_process->readAllStandardOutput()) : QString();
}

bool VideoPlayer::play(const QString &url) {
    QString command = Settings::videoPlayerCommand();
    
    if (command.isEmpty()) {
        Logger::log("VideoPlayer::play(): No video player command");
        setErrorString(tr("No video player command has been set."));
        setStatus(Failed);
        return false;
    }
    
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError()));
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int)));
        connect(m_process, SIGNAL(readyReadStandardOutput()), this, SIGNAL(readyReadOutput()));
    }
    
    command.replace("%u", url);
    Logger::log(QString("VideoPlayer::play(): URL: %1. Command: %2").arg(url).arg(command));
    setErrorString(QString());
    setStatus(Playing);
    m_process->start(command);
    return true;
}

bool VideoPlayer::stop() {
    if ((m_process) && (m_process->state() == QProcess::Running)) {
        Logger::log("VideoPlayer::stop(): Terminating player process");
        m_process->terminate();
        return true;
    }
    
    return false;
}

void VideoPlayer::onProcessError() {
    Logger::log("VideoPlayer::onProcessError(): " + m_process->errorString());
    setErrorString(m_process->errorString());
    setStatus(Failed);
    emit finished();
}

void VideoPlayer::onProcessFinished(int exitCode) {
    Logger::log("VideoPlayer::onProcessFinished(): Exit code: " + QString::number(exitCode));
    setErrorString(m_process->errorString());
    setStatus(exitCode == 0 ? Null : Failed);
    emit finished();
}
