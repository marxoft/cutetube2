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

#ifndef VIMEOUSERWINDOW_H
#define VIMEOUSERWINDOW_H

#include "stackedwindow.h"
#include "vimeouser.h"

class Image;
class ListView;
class TextBrowser;
class QScrollArea;
class QLabel;
class QPushButton;
class QHBoxLayout;
class QModelIndex;
class QStringListModel;

class VimeoUserWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit VimeoUserWindow(const QString &id, StackedWindow *parent = 0);
    explicit VimeoUserWindow(const VimeoUser *user, StackedWindow *parent = 0);
    
private:
    void loadBaseUi();
    void loadUserUi();
    
    void showFavourites();
    void showPlaylists();
    void showSubscriptions();
    void showUploads();
    
private Q_SLOTS:
    void setUserSubscription();

    void showResource(const QUrl &url);    
    void showWindow(const QModelIndex &index);
    
    void onUserStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onUserUpdateStatusChanged(QVimeo::ResourcesRequest::Status status);
    void onUserSubscribedChanged();

private:
    VimeoUser *m_user;
    QStringListModel *m_model;
    
    Image *m_avatar;
    ListView *m_view;
    QScrollArea *m_scrollArea;
    QLabel *m_titleLabel;
    QLabel *m_statsLabel;
    TextBrowser *m_descriptionLabel;
    QPushButton *m_subscribeButton;
    QHBoxLayout *m_layout;
};

#endif // VIMEOUSERWINDOW_H
