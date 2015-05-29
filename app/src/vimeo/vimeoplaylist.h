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

#ifndef VIMEOPLAYLIST_H
#define VIMEOPLAYLIST_H

#include "playlist.h"
#include <qvimeo/resourcesrequest.h>

class VimeoVideo;

class VimeoPlaylist : public CTPlaylist
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString password READ password NOTIFY passwordChanged)
    Q_PROPERTY(QString privacy READ privacy NOTIFY privacyChanged) 
    Q_PROPERTY(QVimeo::ResourcesRequest::Status status READ status NOTIFY statusChanged)

public:
    explicit VimeoPlaylist(QObject *parent = 0);
    explicit VimeoPlaylist(const QString &id, QObject *parent = 0);
    explicit VimeoPlaylist(const QVariantMap &playlist, QObject *parent = 0);
    explicit VimeoPlaylist(const VimeoPlaylist *playlist, QObject *parent = 0);
    
    QString errorString() const;

    QString password() const;    
    QString privacy() const;
        
    QVimeo::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadPlaylist(const QString &id);
    Q_INVOKABLE void loadPlaylist(const QVariantMap &playlist);
    
public Q_SLOTS:
    void addVideo(VimeoVideo *video);
    void addVideo(const QVariantMap &playlist, VimeoVideo *video);
    void removeVideo(VimeoVideo *video);
    
private:
    void initRequest();
    
    void setPassword(const QString &p);
    void setPrivacy(const QString &p);
            
private Q_SLOTS:
    void onPlaylistRequestFinished();
    void onCreatePlaylistRequestFinished();
    void onAddVideoRequestFinished();
    void onRemoveVideoRequestFinished();
    void onPlaylistUpdated(VimeoVideo*, VimeoPlaylist*);
    
Q_SIGNALS:
    void passwordChanged();
    void privacyChanged();
    void statusChanged(QVimeo::ResourcesRequest::Status s);

private:
    QVimeo::ResourcesRequest *m_request;
    VimeoVideo *m_video;
    
    QString m_password;
    QString m_privacy;
};

#endif // VIMEOPLAYLIST_H
