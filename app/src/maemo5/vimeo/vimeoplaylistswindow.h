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

#ifndef VIMEOPLAYLISTSWINDOW_H
#define VIMEOPLAYLISTSWINDOW_H

#include "stackedwindow.h"
#include "vimeoplaylistmodel.h"

class PlaylistDelegate;
class ImageCache;
class ListView;
class QLabel;
class QVBoxLayout;

class VimeoPlaylistsWindow : public StackedWindow
{
    Q_OBJECT
    
public:
    explicit VimeoPlaylistsWindow(StackedWindow *parent = 0);
    ~VimeoPlaylistsWindow();

public Q_SLOTS:
    void list(const QString &resourcePath, const QVariantMap &filters = QVariantMap());
    
private Q_SLOTS:
    void showPlaylist(const QModelIndex &index);
        
    void onImageReady();
    void onModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    
private:
    VimeoPlaylistModel *m_model;
    ImageCache *m_cache;
    
    ListView *m_view;
    PlaylistDelegate *m_delegate;
    QAction *m_reloadAction;
    QLabel *m_label;
    QVBoxLayout *m_layout;
};
    
#endif // VIMEOPLAYLISTSWINDOW_H
