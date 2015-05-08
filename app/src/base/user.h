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

#ifndef USER_H
#define USER_H

#include <QObject>
#include <QUrl>

class User : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString id READ id NOTIFY idChanged)
    Q_PROPERTY(QUrl largeThumbnailUrl READ largeThumbnailUrl NOTIFY largeThumbnailUrlChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(QUrl thumbnailUrl READ thumbnailUrl NOTIFY thumbnailUrlChanged)
    Q_PROPERTY(QString username READ username NOTIFY usernameChanged)
    
public:
    explicit User(QObject *parent = 0);
    explicit User(const User *user, QObject *parent = 0);
            
    QString description() const;
            
    QString id() const;
    
    QUrl largeThumbnailUrl() const;
            
    QString service() const;
    
    QUrl thumbnailUrl() const;
            
    QString username() const;
    
    Q_INVOKABLE virtual void loadUser(const User *user);
    
protected:    
    void setDescription(const QString &d);
        
    void setId(const QString &i);
    
    void setLargeThumbnailUrl(const QUrl &u);
    
    void setService(const QString &s);
    
    void setThumbnailUrl(const QUrl &u);
            
    void setUsername(const QString &u);

    
Q_SIGNALS:
    void descriptionChanged();
    void idChanged();
    void largeThumbnailUrlChanged();
    void serviceChanged();
    void thumbnailUrlChanged();
    void usernameChanged();

protected:
    QString m_description;
    QString m_id;
    QUrl m_largeThumbnailUrl;
    QString m_service;
    QUrl m_thumbnailUrl;
    QString m_username;
};

#endif // USER_H
