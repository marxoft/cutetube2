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

#ifndef YOUTUBEUSER_H
#define YOUTUBEUSER_H

#include "user.h"
#include <qyoutube/resourcesrequest.h>

class YouTubeUser : public CTUser
{
    Q_OBJECT
    
    Q_PROPERTY(QUrl bannerUrl READ bannerUrl NOTIFY bannerUrlChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QUrl largeBannerUrl READ largeBannerUrl NOTIFY largeBannerUrlChanged)
    Q_PROPERTY(QVariantMap relatedPlaylists READ relatedPlaylists NOTIFY relatedPlaylistsChanged)
    Q_PROPERTY(QYouTube::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(bool subscribed READ isSubscribed NOTIFY subscribedChanged)
    Q_PROPERTY(QString subscriptionId READ subscriptionId NOTIFY subscriptionIdChanged)
    Q_PROPERTY(qint64 subscriberCount READ subscriberCount NOTIFY subscriberCountChanged)
    Q_PROPERTY(qint64 viewCount READ viewCount NOTIFY viewCountChanged)
    
public:
    explicit YouTubeUser(QObject *parent = 0);
    explicit YouTubeUser(const QString &id, QObject *parent = 0);
    explicit YouTubeUser(const QVariantMap &user, QObject *parent = 0);
    explicit YouTubeUser(const YouTubeUser *user, QObject *parent = 0);
    
    QUrl bannerUrl() const;
    
    QString errorString() const;
    
    QUrl largeBannerUrl() const;
    
    bool isSubscribed() const;
    
    QVariantMap relatedPlaylists() const;
    
    QYouTube::ResourcesRequest::Status status() const;
    
    QString subscriptionId() const;
    
    qint64 subscriberCount() const;
        
    qint64 viewCount() const;
    
    Q_INVOKABLE void loadUser(const QString &id);
    Q_INVOKABLE void loadUser(const QVariantMap &user);
    Q_INVOKABLE void loadUser(YouTubeUser *user);

public Q_SLOTS:
    void checkIfSubscribed();
    void subscribe();
    void unsubscribe();
    
private:
    void initRequest();
    
    void setBannerUrl(const QUrl &u);
    
    void setLargeBannerUrl(const QUrl &u);
    
    void setRelatedPlaylists(const QVariantMap &p);
    
    void setSubscribed(bool s);
    
    void setSubscriptionId(const QString &i);
    
    void setSubscriberCount(qint64 c);
        
    void setViewCount(qint64 c);
        
private Q_SLOTS:
    void onUserRequestFinished();
    void onSubscribeCheckRequestFinished();
    void onSubscribeRequestFinished();
    void onUnsubscribeRequestFinished();
    void onUserUpdated(YouTubeUser *user);
    
Q_SIGNALS:
    void bannerUrlChanged();
    void largeBannerUrlChanged();
    void relatedPlaylistsChanged();
    void statusChanged(QYouTube::ResourcesRequest::Status s);
    void subscribedChanged();
    void subscriptionIdChanged();
    void subscriberCountChanged();
    void viewCountChanged();
    
private:
    QYouTube::ResourcesRequest *m_request;
    
    QUrl m_bannerUrl;
    QUrl m_largeBannerUrl;
    QVariantMap m_relatedPlaylists;
    bool m_subscribed;
    QString m_subscriptionId;
    qint64 m_subscriberCount;
    qint64 m_viewCount;
};
    
#endif // YOUTUBEUSER_H
