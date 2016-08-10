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

#ifndef VIDEOPLAYER_H
#define VIDEOPLAYER_H

#include <QObject>

class QProcess;

class VideoPlayer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    
    Q_ENUMS(Status)

public:
    enum Status {
        Null = 0,
        Playing,
        Failed
    };
    
    explicit VideoPlayer(QObject *parent = 0);
    
    QString errorString() const;
    
    Status status() const;
        
public Q_SLOTS:
    bool play(const QString &url);
    bool stop();
    
    QString readAllOutput() const;

private Q_SLOTS:
    void onProcessError();
    void onProcessFinished(int exitCode);

Q_SIGNALS:
    void finished();
    void readyReadOutput();
    void statusChanged(VideoPlayer::Status status);

private:
    void setErrorString(const QString &e);
    
    void setStatus(Status s);
    
    QProcess *m_process;
    
    QString m_errorString;
    
    Status m_status;
};

#endif // VIDEOPLAYER_H
