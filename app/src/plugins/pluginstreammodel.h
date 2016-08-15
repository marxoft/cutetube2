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

#ifndef PLUGINSTREAMMODEL_H
#define PLUGINSTREAMMODEL_H

#include "selectionmodel.h"
#include "resourcesrequest.h"

namespace QDailymotion {
    class StreamsRequest;
}

namespace QVimeo {
    class StreamsRequest;
}

namespace QYouTube {
    class StreamsRequest;
}

class PluginStreamModel : public SelectionModel
{
    Q_OBJECT
    
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(ResourcesRequest::Status status READ status NOTIFY statusChanged)
    
public:
    explicit PluginStreamModel(QObject *parent = 0);

    QString errorString() const;
    
    QString service() const;
    void setService(const QString &s);
    
    ResourcesRequest::Status status() const;
    
    Q_INVOKABLE void list(const QString &resourceId);
        
public Q_SLOTS:
    void cancel();
    void reload();

private Q_SLOTS:
    void onRequestFinished();
    void onDailymotionRequestFinished();
    void onVimeoRequestFinished();
    void onYouTubeRequestFinished();
    
Q_SIGNALS:
    void serviceChanged();
    void statusChanged(ResourcesRequest::Status s);
        
private:
    void setErrorString(const QString &e);
    
    void setStatus(ResourcesRequest::Status s);
    
    ResourcesRequest* request();
    QDailymotion::StreamsRequest* dailymotionRequest();
    QVimeo::StreamsRequest* vimeoRequest();
    QYouTube::StreamsRequest* youtubeRequest();
    
    ResourcesRequest *m_request;
    QDailymotion::StreamsRequest *m_dailymotionRequest;
    QVimeo::StreamsRequest *m_vimeoRequest;
    QYouTube::StreamsRequest *m_youtubeRequest;

    QString m_errorString;
    QString m_service;
    QString m_resourceId;
    
    ResourcesRequest::Status m_status;
};
    
#endif // PLUGINSTREAMMODEL_H
