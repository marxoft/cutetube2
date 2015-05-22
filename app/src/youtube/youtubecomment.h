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

#ifndef YOUTUBECOMMENT_H
#define YOUTUBECOMMENT_H

#include "comment.h"
#include <qyoutube/resourcesrequest.h>

class YouTubeComment : public Comment
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString parentId READ parentId NOTIFY parentIdChanged)
    Q_PROPERTY(QYouTube::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit YouTubeComment(QObject *parent = 0);
    explicit YouTubeComment(const QString &id, QObject *parent = 0);
    explicit YouTubeComment(const QVariantMap &comment, QObject *parent = 0);
    explicit YouTubeComment(const YouTubeComment *comment, QObject *parent = 0);
    
    QString errorString() const;
    
    QString parentId() const;
        
    QYouTube::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadComment(const QString &id);
    Q_INVOKABLE void loadComment(const QVariantMap &comment);
    Q_INVOKABLE void loadComment(YouTubeComment *comment);

public Q_SLOTS:
    void addComment();
    void addComment(const QVariantMap &comment);
    
private:
    void initRequest();
    
    void setParentId(const QString &i);
        
private Q_SLOTS:
    void onCommentRequestFinished();
    void onAddCommentRequestFinished();
    
Q_SIGNALS:
    void parentIdChanged();
    void statusChanged(QYouTube::ResourcesRequest::Status s);
    
private:
    QYouTube::ResourcesRequest *m_request;
    
    QString m_parentId;
};
    
#endif // YOUTUBECOMMENT_H
