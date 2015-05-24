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

#ifndef DAILYMOTIONVIEW_H
#define DAILYMOTIONVIEW_H

#include <QWidget>

class DailymotionUser;
class DailymotionVideo;
class DailymotionPlaylist;
class DailymotionNavModel;
class ListView;
class QModelIndex;
class QVBoxLayout;

class DailymotionView : public QWidget
{
    Q_OBJECT
    
public:
    explicit DailymotionView(QWidget *parent = 0);
    
public Q_SLOTS:
    void search(const QString &query, const QString &type, const QString &order);
    void showResource(const QString &type, const QString &id);
    
private:
    void showAccounts();
    void showFavourites();
    void showLatestVideos();
    void showPlaylists();
    void showSearchDialog();
    void showSubscriptions();
    void showUploads();
    
private Q_SLOTS:
    void onItemActivated(const QModelIndex &index);
    
    void onCommentAdded();
    
    void onUserSubscribed(DailymotionUser *user);
    void onUserUnsubscribed(DailymotionUser *user);
    
    void onVideoFavourited(DailymotionVideo *video);
    void onVideoUnfavourited(DailymotionVideo *video);
    void onVideoAddedToPlaylist(DailymotionVideo *video, DailymotionPlaylist *playlist);
    
private:
    DailymotionNavModel *m_model;
    
    ListView *m_view;
    QVBoxLayout *m_layout;
};

#endif // DAILYMOTIONVIEW_H
