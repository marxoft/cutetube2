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

#ifndef YOUTUBE_H
#define YOUTUBE_H

#include <QObject>
#include <QStringList>
#include <QVariantMap>
#include <QRegExp>
#include <QUrl>

class YouTubeComment;
class YouTubePlaylist;
class YouTubeUser;
class YouTubeVideo;

class YouTube : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString accessToken READ accessToken WRITE setAccessToken NOTIFY accessTokenChanged)
    Q_PROPERTY(QString refreshToken READ refreshToken WRITE setRefreshToken NOTIFY refreshTokenChanged)
    Q_PROPERTY(QVariantMap relatedPlaylists READ relatedPlaylists WRITE setRelatedPlaylists
               NOTIFY relatedPlaylistsChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(QString clientId READ clientId WRITE setClientId NOTIFY clientIdChanged)
    Q_PROPERTY(QString clientSecret READ clientSecret WRITE setClientSecret NOTIFY clientSecretChanged)
    Q_PROPERTY(QStringList scopes READ scopes WRITE setScopes NOTIFY scopesChanged)
            
public:
    explicit YouTube(QObject *parent = 0);
    ~YouTube();
    
    static const QRegExp URL_REGEXP;
    
    static YouTube* instance();
        
    Q_INVOKABLE static QString formatDuration(const QString &duration);
    
    Q_INVOKABLE static QString getErrorString(const QVariantMap &error);
    
    Q_INVOKABLE static QString getPlaylistId(const QVariantMap &playlist);
    Q_INVOKABLE static QString getUserId(const QVariantMap &user);
    Q_INVOKABLE static QString getVideoId(const QVariantMap &video);
    
    Q_INVOKABLE QString relatedPlaylist(const QString &name) const;
    
    Q_INVOKABLE QUrl authUrl() const;
    
    QString userId() const;
        
    QString accessToken() const;
    
    QString refreshToken() const;
        
    QVariantMap relatedPlaylists() const;
    
    QString apiKey() const;
    
    QString clientId() const;
    
    QString clientSecret() const;
    
    QStringList scopes() const;
    
    Q_INVOKABLE bool hasScope(const QString &scope) const;
    
public Q_SLOTS:
    void setUserId(const QString &id);
    
    void setAccessToken(const QString &token);
    
    void setRefreshToken(const QString &token);
        
    void setRelatedPlaylists(const QVariantMap &playlists);
                
    void setApiKey(const QString &key);
    
    void setClientId(const QString &id);
    
    void setClientSecret(const QString &secret);
    
    void setScopes(const QStringList &s);
    
Q_SIGNALS:
    void userIdChanged();
    void accessTokenChanged();
    void refreshTokenChanged();
    void relatedPlaylistsChanged();
    void apiKeyChanged();
    void clientIdChanged();
    void clientSecretChanged();
    void scopesChanged();
    
    void commentAdded(const YouTubeComment *comment);
    
    void playlistCreated(const YouTubePlaylist *playlist);
    void playlistDeleted(const YouTubePlaylist *playlist);
    
    void userSubscribed(const YouTubeUser *user);
    void userUnsubscribed(const YouTubeUser *user);
    
    void videoAddedToPlaylist(const YouTubeVideo *video, const YouTubePlaylist *playlist);
    void videoRemovedFromPlaylist(const YouTubeVideo *video, const YouTubePlaylist *playlist);
    void videoDisliked(const YouTubeVideo *video);
    void videoLiked(const YouTubeVideo *video);
    void videoFavourited(const YouTubeVideo *video);
    void videoUnfavourited(const YouTubeVideo *video);
    void videoWatchLater(const YouTubeVideo *video);

private:
    struct SubscriptionCache {
        QHash<QString, QString> ids;
        QString nextPageToken;
    };
    
    static SubscriptionCache subscriptionCache;
    static YouTube *self;
    
    friend class YouTubeComment;
    friend class YouTubePlaylist;
    friend class YouTubeUser;
    friend class YouTubeVideo;  
};

#endif // YOUTUBE_H
