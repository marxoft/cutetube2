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

#ifndef YOUTUBEUSERPAGE_H
#define YOUTUBEUSERPAGE_H

#include "page.h"
#include "youtubeuser.h"

class Banner;
class TextBrowser;
class QToolBar;
class QVBoxLayout;

class YouTubeUserPage : public Page
{
    Q_OBJECT

public:
    explicit YouTubeUserPage(const QString &userId, QWidget *parent = 0);
    explicit YouTubeUserPage(const YouTubeUser *user, QWidget *parent = 0);
    
    virtual Status status() const;
    virtual QString statusText() const;

public Q_SLOTS:
    virtual void reload();

private Q_SLOTS:
    void showFavourites();
    void showPlaylists();
    void showSubscriptions();
    void showUploads();
    
    void onUserLoadingStatusChanged(QYouTube::ResourcesRequest::Status status);

private:
    void setStatus(Status s);
    void setStatusText(const QString &text);
    
    void loadUi();
    
    YouTubeUser *m_user;    
    
    Banner *m_banner;
    
    QToolBar *m_toolBar;
    
    QAction *m_uploadsAction;
    QAction *m_favouritesAction;
    QAction *m_playlistsAction;
    QAction *m_subscriptionsAction;
    QAction *m_subscribeAction;
    
    TextBrowser *m_descriptionLabel;
    
    QVBoxLayout *m_layout;
    
    Status m_status;
    
    QString m_statusText;
};
    
#endif // YOUTUBEUSERPAGE_H
