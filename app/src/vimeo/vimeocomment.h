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

#ifndef VIMEOCOMMENT_H
#define VIMEOCOMMENT_H

#include "comment.h"
#include <qvimeo/resourcesrequest.h>

class VimeoComment : public Comment
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QVimeo::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit VimeoComment(QObject *parent = 0);
    explicit VimeoComment(const QString &id, QObject *parent = 0);
    explicit VimeoComment(const QVariantMap &comment, QObject *parent = 0);
    explicit VimeoComment(const VimeoComment *comment, QObject *parent = 0);
    
    QString errorString() const;
            
    QVimeo::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadComment(const QString &id);
    Q_INVOKABLE void loadComment(const QVariantMap &comment);

public Q_SLOTS:
    void addComment();
    
private:
    void initRequest();
        
private Q_SLOTS:
    void onCommentRequestFinished();
    void onAddCommentRequestFinished();
    
Q_SIGNALS:
    void statusChanged(QVimeo::ResourcesRequest::Status s);
    
private:
    QVimeo::ResourcesRequest *m_request;    
};
    
#endif // VIMEOCOMMENT_H
