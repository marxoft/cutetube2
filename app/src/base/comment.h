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

#ifndef COMMENT_H
#define COMMENT_H

#include <QObject>
#include <QUrl>

class CTComment : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString body READ body NOTIFY bodyChanged)
    Q_PROPERTY(QString date READ date NOTIFY dateChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString userId READ userId NOTIFY userIdChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    Q_PROPERTY(QString videoId READ videoId NOTIFY videoIdChanged)
    
public:
    explicit CTComment(QObject *parent = 0);
    explicit CTComment(const CTComment *comment, QObject *parent = 0);
            
    QString body() const;
    
    QString date() const;
            
    QString id() const;
                
    QString service() const;
    
    QUrl thumbnailUrl() const;
        
    QString userId() const;
        
    QString username() const;
    
    QString videoId() const;
    
    Q_INVOKABLE virtual void loadComment(CTComment *comment);
    
protected:    
    void setBody(const QString &b);
    
    void setDate(const QString &d);
        
    void setId(const QString &i);
        
    void setService(const QString &s);
    
    void setThumbnailUrl(const QUrl &u);
        
    void setUserId(const QString &i);
        
    void setUsername(const QString &u);
    
    void setVideoId(const QString &i);

    
Q_SIGNALS:
    void bodyChanged();
    void dateChanged();
    void idChanged();
    void serviceChanged();
    void thumbnailUrlChanged();
    void userIdChanged();
    void usernameChanged();
    void videoIdChanged();

protected:
    QString m_body;
    QString m_date;
    QString m_id;
    QString m_service;
    QUrl m_thumbnailUrl;
    QString m_userId;
    QString m_username;
    QString m_videoId;
};

#endif // COMMENT_H
