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

#ifndef PLUGINUSERWINDOW_H
#define PLUGINUSERWINDOW_H

#include "stackedwindow.h"
#include "pluginuser.h"

class Image;
class ListView;
class TextBrowser;
class QScrollArea;
class QLabel;
class QHBoxLayout;
class QModelIndex;
class QStringListModel;

class PluginUserWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit PluginUserWindow(const QString &service, const QString &id, StackedWindow *parent = 0);
    explicit PluginUserWindow(const PluginUser *user, StackedWindow *parent = 0);
    
private:
    void loadBaseUi();
    void loadUserUi();
    
    void showPlaylists();
    void showVideos();
    
private Q_SLOTS:
    void showResource(const QUrl &url);
    void showWindow(const QModelIndex &index);
    
    void onUserStatusChanged(ResourcesRequest::Status status);

private:
    PluginUser *m_user;
    QStringListModel *m_model;
    
    Image *m_avatar;
    ListView *m_view;
    QScrollArea *m_scrollArea;
    QLabel *m_titleLabel;
    TextBrowser *m_descriptionLabel;
    QHBoxLayout *m_layout;
};

#endif // PLUGINUSERWINDOW_H
