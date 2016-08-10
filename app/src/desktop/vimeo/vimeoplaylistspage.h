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

#ifndef VIMEOPLAYLISTSPAGE_H
#define VIMEOPLAYLISTSPAGE_H

#include "page.h"
#include "vimeoplaylistmodel.h"

class ImageCache;
class PlaylistDelegate;
class QListView;
class QVBoxLayout;

class VimeoPlaylistsPage : public Page
{
    Q_OBJECT

public:
    explicit VimeoPlaylistsPage(QWidget *parent = 0);
    ~VimeoPlaylistsPage();

    Status status() const;

public Q_SLOTS:
    virtual void cancel();
    virtual void reload();

    void list(const QString &resourcePath, const QVariantMap &filters = QVariantMap());

private Q_SLOTS:    
    void onImageReady();
    void onModelStatusChanged(QVimeo::ResourcesRequest::Status status);

private:
    void setStatus(Status s);
    
    VimeoPlaylistModel *m_model;

    ImageCache *m_cache;

    PlaylistDelegate *m_delegate;
    
    QListView *m_view;

    QVBoxLayout *m_layout;

    Status m_status;
};

#endif // VIMEOPLAYLISTSPAGE_H
