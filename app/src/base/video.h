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

#ifndef VIDEO_H
#define VIDEO_H

#include <QObject>
#include <QUrl>

class CTVideo : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(bool downloadable READ isDownloadable NOTIFY downloadableChanged)
    Q_PROPERTY(QString duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY largeThumbnailUrlChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QUrl streamUrl READ streamUrl NOTIFY streamUrlChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString title READ title NOTIFY titleChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY urlChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY userIdChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(qint64 viewCount READ viewCount NOTIFY viewCountChanged)
    
public:
    explicit CTVideo(QObject *parent = 0);
    explicit CTVideo(const CTVideo *video, QObject *parent = 0);
        
    QString date() const;
    
    QString description() const;
        
    QString duration() const;
        
    QString id() const;
    
    bool isDownloadable() const;
    
    QUrl largeThumbnailUrl() const;
            
    QString service() const;
    
    QUrl streamUrl() const;
    
    QUrl thumbnailUrl() const;
    
    QString title() const;
    
    QUrl url() const;
    
    QString userId() const;
    
    QString username() const;
        
    qint64 viewCount() const;
    
    Q_INVOKABLE virtual void loadVideo(CTVideo *video);
    
public Q_SLOTS:
    virtual void viewed();
    
protected:
    void setDate(const QString &d);
    
    void setDescription(const QString &d);
    
    void setDownloadable(bool d);
    
    void setDuration(const QString &d);
    
    void setId(const QString &i);
    
    void setLargeThumbnailUrl(const QUrl &u);
    
    void setService(const QString &s);
    
    void setStreamUrl(const QUrl &u);
    
    void setThumbnailUrl(const QUrl &u);
    
    void setTitle(const QString &t);
    
    void setUrl(const QUrl &u);
    
    void setUserId(const QString &i);
    
    void setUsername(const QString &u);
        
    void setViewCount(qint64 c);

    
Q_SIGNALS:
    void dateChanged();
    void descriptionChanged();
    void downloadableChanged();
    void durationChanged();
    void idChanged();
    void largeThumbnailUrlChanged();
    void serviceChanged();
    void streamUrlChanged();
    void thumbnailUrlChanged();
    void titleChanged();
    void urlChanged();
    void userIdChanged();
    void usernameChanged();
    void viewCountChanged();

protected:
    QString m_date;
    QString m_description;
    bool m_downloadable;
    QString m_duration;
    QString m_id;
    QUrl m_largeThumbnailUrl;
    QString m_service;
    QUrl m_streamUrl;
    QUrl m_thumbnailUrl;
    QString m_title;
    QUrl m_url;
    QString m_userId;
    QString m_username;
    qint64 m_viewCount;
};

#endif // VIDEO_H
