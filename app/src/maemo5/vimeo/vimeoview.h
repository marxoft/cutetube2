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

#ifndef VIMEOVIEW_H
#define VIMEOVIEW_H

#include <QWidget>

class VimeoUser;
class VimeoVideo;
class VimeoPlaylist;
class VimeoNavModel;
class ListView;
class QModelIndex;
class QVBoxLayout;

class VimeoView : public QWidget
{
    Q_OBJECT
    
public:
    explicit VimeoView(QWidget *parent = 0);
    
public Q_SLOTS:
    void search(const QString &query, const QString &type, const QString &order);
    void showResource(const QString &type, const QString &id);
    
private:
    void showAccounts();
    void showCategories();
    void showFavourites();
    void showLatestVideos();
    void showPlaylists();
    void showSearchDialog();
    void showSubscriptions();
    void showUploads();
    void showWatchLater();
    
private Q_SLOTS:
    void onItemActivated(const QModelIndex &index);
    
    void onCommentAdded();
    
    void onUserSubscribed(VimeoUser *user);
    void onUserUnsubscribed(VimeoUser *user);
    
    void onVideoFavourited(VimeoVideo *video);
    void onVideoUnfavourited(VimeoVideo *video);
    void onVideoWatchLater(VimeoVideo *video);
    void onVideoAddedToPlaylist(VimeoVideo *video, VimeoPlaylist *playlist);
    
private:
    VimeoNavModel *m_model;
    
    ListView *m_view;
    QVBoxLayout *m_layout;
};

#endif // VIMEOVIEW_H
