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

#ifndef DAILYMOTIONCOMMENT_H
#define DAILYMOTIONCOMMENT_H

#include "comment.h"
#include <qdailymotion/resourcesrequest.h>

class DailymotionComment : public CTComment
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QDailymotion::ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit DailymotionComment(QObject *parent = 0);
    explicit DailymotionComment(const QString &id, QObject *parent = 0);
    explicit DailymotionComment(const QVariantMap &comment, QObject *parent = 0);
    explicit DailymotionComment(const DailymotionComment *comment, QObject *parent = 0);
    
    QString errorString() const;
        
    QDailymotion::ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void loadComment(const QString &id);
    Q_INVOKABLE void loadComment(const QVariantMap &comment);

public Q_SLOTS:
    void addComment();
    void addComment(const QVariantMap &comment);
    
private:
    void initRequest();
        
private Q_SLOTS:
    void onCommentRequestFinished();
    void onAddCommentRequestFinished();
    
Q_SIGNALS:
    void statusChanged(QDailymotion::ResourcesRequest::Status s);
    
private:
    QDailymotion::ResourcesRequest *m_request;
};
    
#endif // DAILYMOTIONCOMMENT_H
