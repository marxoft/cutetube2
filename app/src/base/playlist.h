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

#ifndef PLAYLIST_H
#define PLAYLIST_H

#include <QObject>
#include <QUrl>

class CTPlaylist : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY largeThumbnailUrlChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY userIdChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(int videoCount READ videoCount NOTIFY videoCountChanged)
    
public:
    explicit CTPlaylist(QObject *parent = 0);
    explicit CTPlaylist(const CTPlaylist *playlist, QObject *parent = 0);
        
    QString date() const;
    
    QString description() const;
            
    QString id() const;
    
    QUrl largeThumbnailUrl() const;
            
    QString service() const;
    
    QUrl thumbnailUrl() const;
    
    QString title() const;
        
    QString userId() const;
    
    QString username() const;
        
    int videoCount() const;
    
    Q_INVOKABLE virtual void loadPlaylist(CTPlaylist *playlist);
    
protected:
    void setDate(const QString &d);
    
    void setDescription(const QString &d);
        
    void setId(const QString &i);
    
    void setLargeThumbnailUrl(const QUrl &u);
    
    void setService(const QString &s);
    
    void setThumbnailUrl(const QUrl &u);
    
    void setTitle(const QString &t);
        
    void setUserId(const QString &i);
    
    void setUsername(const QString &u);
        
    void setVideoCount(int c);

    
Q_SIGNALS:
    void dateChanged();
    void descriptionChanged();
    void idChanged();
    void largeThumbnailUrlChanged();
    void serviceChanged();
    void thumbnailUrlChanged();
    void titleChanged();
    void userIdChanged();
    void usernameChanged();
    void videoCountChanged();

protected:
    QString m_date;
    QString m_description;
    QString m_id;
    QUrl m_largeThumbnailUrl;
    QString m_service;
    QUrl m_thumbnailUrl;
    QString m_title;
    QString m_userId;
    QString m_username;
    int m_videoCount;    
};

#endif // PLAYLIST_H
