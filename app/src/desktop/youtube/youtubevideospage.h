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

#ifndef YOUTUBEVIDEOSPAGE_H
#define YOUTUBEVIDEOSPAGE_H

#include "page.h"
#include "youtubevideomodel.h"

class ImageCache;
class VideoDelegate;
class QListView;
class QVBoxLayout;

class YouTubeVideosPage : public Page
{
    Q_OBJECT

public:
    explicit YouTubeVideosPage(QWidget *parent = 0);
    ~YouTubeVideosPage();

    virtual Status status() const;
    virtual QString statusText() const;

public Q_SLOTS:
    virtual void cancel();
    virtual void reload();
    
    void list(const QString &resourcePath, const QStringList &part,
              const QVariantMap &filters = QVariantMap(), const QVariantMap &params = QVariantMap());

private Q_SLOTS:
    void downloadVideo(const QModelIndex &index);
    void playVideo(const QModelIndex &index);
    void showVideo(const QModelIndex &index);

    void showContextMenu(const QPoint &pos);
        
    void onImageReady();
    void onModelStatusChanged(QYouTube::ResourcesRequest::Status status);

private:
    void setStatus(Status s);
    void setStatusText(const QString &text);
    
    YouTubeVideoModel *m_model;

    ImageCache *m_cache;

    VideoDelegate *m_delegate;

    QListView *m_view;

    QVBoxLayout *m_layout;

    Status m_status;
    
    QString m_statusText;
};

#endif // YOUTUBEVIDEOSPAGE_H
