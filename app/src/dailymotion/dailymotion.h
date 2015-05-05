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

#ifndef DAILYMOTION_H
#define DAILYMOTION_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QRegExp>
#include <QUrl>

class DailymotionComment;
class DailymotionPlaylist;
class DailymotionUser;
class DailymotionVideo;

class Dailymotion : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString refreshToken READ refreshToken WRITE setRefreshToken NOTIFY refreshTokenChanged)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    Q_PROPERTY(QString redirectUri READ redirectUri WRITE setRedirectUri NOTIFY redirectUriChanged)
    Q_PROPERTY(QStringList scopes READ scopes WRITE setScopes NOTIFY scopesChanged)
        
public:
    explicit Dailymotion(QObject *parent = 0);
    ~Dailymotion();
    
    static const QStringList COMMENT_FIELDS;
    static const QStringList PLAYLIST_FIELDS;
    static const QStringList USER_FIELDS;
    static const QStringList VIDEO_FIELDS;
    
    static const QRegExp URL_REGEXP;
    
    static Dailymotion* instance();
            
    Q_INVOKABLE static QString getErrorString(const QVariantMap &error);
    
    Q_INVOKABLE QUrl authUrl() const;
    
    QString userId() const;
    
    QString accessToken() const;
    
    QString refreshToken() const;
            
    QString clientId() const;
    
    QString clientSecret() const;
    
    QString redirectUri() const;
    
    QStringList scopes() const;
    
    Q_INVOKABLE bool hasScope(const QString &scope) const;
    
public Q_SLOTS:    
    void setUserId(const QString &id);
    
    void setAccessToken(const QString &token);
    
    void setRefreshToken(const QString &token);
    
    void setClientId(const QString &id);
    
    void setClientSecret(const QString &secret);
    
    void setRedirectUri(const QString &uri);
    
    void setScopes(const QStringList &s);
    
Q_SIGNALS:
    void userIdChanged();
    void accessTokenChanged();
    void refreshTokenChanged();
    void clientIdChanged();
    void clientSecretChanged();
    void redirectUriChanged();
    void scopesChanged();

    void commentAdded(const DailymotionComment *comment);
    
    void playlistCreated(const DailymotionPlaylist *playlist);
    void playlistDeleted(const DailymotionPlaylist *playlist);
    
    void userSubscribed(const DailymotionUser *user);
    void userUnsubscribed(const DailymotionUser *user);
    
    void videoAddedToPlaylist(const DailymotionVideo *video, const DailymotionPlaylist *playlist);
    void videoRemovedFromPlaylist(const DailymotionVideo *video, const DailymotionPlaylist *playlist);
    void videoFavourited(const DailymotionVideo *video);
    void videoUnfavourited(const DailymotionVideo *video);

private:
    struct SubscriptionCache {
        QStringList ids;
        QVariantMap filters;
        bool hasMore;
    };
    
    static SubscriptionCache subscriptionCache;
    static Dailymotion *self;
    
    friend class DailymotionComment;
    friend class DailymotionPlaylist;
    friend class DailymotionUser;
    friend class DailymotionVideo;  
};

#endif // DAILYMOTION_H
