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

#ifndef VIMEOUSER_H
#define VIMEOUSER_H

#include "user.h"
#include <qvimeo/resourcesrequest.h>
#include <QStringList>

class VimeoUser : public CTUser
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QVimeo::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool subscribed READ isSubscribed NOTIFY subscribedChanged)
    Q_PROPERTY(qint64 subscriberCount READ subscriberCount NOTIFY subscriberCountChanged)
    
public:
    explicit VimeoUser(QObject *parent = 0);
    explicit VimeoUser(const QString &id, QObject *parent = 0);
    explicit VimeoUser(const QVariantMap &user, QObject *parent = 0);
    explicit VimeoUser(const VimeoUser *user, QObject *parent = 0);
        
    QString errorString() const;
        
    bool isSubscribed() const;
        
    QVimeo::ResourcesRequest::Status status() const;
        
    qint64 subscriberCount() const;
            
    Q_INVOKABLE void loadUser(const QString &id);
    Q_INVOKABLE void loadUser(const QVariantMap &user);
    Q_INVOKABLE void loadUser(VimeoUser *user);

public Q_SLOTS:
    void checkIfSubscribed();
    void subscribe();
    void unsubscribe();
    
private:        
    void initRequest();
    
    void setSubscribed(bool s);
        
    void setSubscriberCount(qint64 c);
            
private Q_SLOTS:
    void onUserRequestFinished();
    void onSubscribeCheckRequestFinished();
    void onSubscribeRequestFinished();
    void onUnsubscribeRequestFinished();
    void onUserUpdated(VimeoUser *user);
    
Q_SIGNALS:
    void statusChanged(QVimeo::ResourcesRequest::Status s);
    void subscribedChanged();
    void subscriberCountChanged();
    
private:    
    QVimeo::ResourcesRequest *m_request;
    
    bool m_subscribed;
    qint64 m_subscriberCount;
};
    
#endif // VIMEOUSER_H
