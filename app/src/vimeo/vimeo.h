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

#ifndef VIMEO_H
#define VIMEO_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QRegExp>
#include <QUrl>

class VimeoComment;
class VimeoPlaylist;
class VimeoUser;
class VimeoVideo;

class Vimeo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    Q_PROPERTY(QString redirectUri READ redirectUri WRITE setRedirectUri NOTIFY redirectUriChanged)
    Q_PROPERTY(QStringList scopes READ scopes WRITE setScopes NOTIFY scopesChanged)
        
public:
    explicit Vimeo(QObject *parent = 0);
    ~Vimeo();
    
    static const QRegExp URL_REGEXP;
    
    static Vimeo* instance();
        
    Q_INVOKABLE static QString getErrorString(const QVariantMap &error);
    
    Q_INVOKABLE QUrl authUrl() const;
    
    QString userId() const;
    
    QString accessToken() const;
            
    QString clientId() const;
    
    QString clientSecret() const;
    
    QString redirectUri() const;
    
    QStringList scopes() const;
    
    Q_INVOKABLE bool hasScope(const QString &scope) const;
    
public Q_SLOTS:
    void setUserId(const QString &id);
    
    void setAccessToken(const QString &token);
    
    void setClientId(const QString &id);
    
    void setClientSecret(const QString &secret);
    
    void setRedirectUri(const QString &uri);
    
    void setScopes(const QStringList &s);
        
Q_SIGNALS:
    void userIdChanged();
    void accessTokenChanged();
    void clientIdChanged();
    void clientSecretChanged();
    void redirectUriChanged();
    void scopesChanged();
    
    void commentAdded(const VimeoComment *comment);
    
    void playlistCreated(const VimeoPlaylist *playlist);
    void playlistDeleted(const VimeoPlaylist *playlist);
    
    void userSubscribed(const VimeoUser *user);
    void userUnsubscribed(const VimeoUser *user);
    
    void videoAddedToPlaylist(const VimeoVideo *video, const VimeoPlaylist *playlist);
    void videoRemovedFromPlaylist(const VimeoVideo *video, const VimeoPlaylist *playlist);
    void videoFavourited(const VimeoVideo *video);
    void videoUnfavourited(const VimeoVideo *video);
    void videoWatchLater(const VimeoVideo *video);

private:
    struct SubscriptionCache {
        QStringList ids;
        QVariantMap filters;
        bool hasMore;
    };
    
    static SubscriptionCache subscriptionCache;
    static Vimeo *self;
    
    friend class VimeoComment;
    friend class VimeoPlaylist;
    friend class VimeoUser;
    friend class VimeoVideo;  
};

#endif // VIMEO_H
