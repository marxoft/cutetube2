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

#ifndef TVREQUEST_H
#define TVREQUEST_H

#include "resourcesrequest.h"
#include <QRegExp>
#include <QVariantList>

class QNetworkAccessManager;
class QNetworkReply;
class QSqlDatabase;
class QSqlQuery;

class TVRequest : public ResourcesRequest
{
    Q_OBJECT

public:
    explicit TVRequest(QObject *parent = 0);
    
    virtual QString errorString() const;

    virtual QVariant result() const;

    virtual ResourcesRequest::Status status() const;

public Q_SLOTS:
    virtual bool cancel();
    virtual bool get(const QString &resourceType, const QString &resourceId);
    virtual bool list(const QString &resourceType, const QString &resourceId);
    virtual bool search(const QString &resourceType, const QString &query, const QString &order);

private Q_SLOTS:
    void checkStreams();

private:
    static void initDatabase();
    static QSqlDatabase getDatabase();
    static QVariantMap getChannel(const QSqlQuery &query);
    static QString getChannelId(const QString &id);
    static QVariantMap getGroup(const QSqlQuery &query);
    static QString getGroupId(const QString &id);
    
    static QString getRedirect(const QNetworkReply *reply);
        
    void setErrorString(const QString &e);

    void setResult(const QVariant &r);

    void setStatus(ResourcesRequest::Status s);
    
    void getChannel(const QString &id);
    void listChannels(const QString &idOrQuery);
    void listChannelsByGroup(const QString &id);
    void listChannelsByQuery(const QString &queryString);
    void searchChannels(const QString &query, const QString &order);
    
    void listGroups(const QString &queryString);
    
    void listStreams(const QString &id, const QString &protocol);
    
    void followRedirect(const QString &url, const char *slot);
    
    QNetworkAccessManager* networkAccessManager();
    
    static const QString API_URL;
    static const QString CHANNEL_URL;
    static const QString LOGO_URL;
    static const QString DATABASE_NAME;
    
    static const QRegExp DATABASE_LIMIT;
        
    static const int MAX_REDIRECTS;
    
    static bool dbinit;
    
    QNetworkAccessManager *m_nam;

    QString m_errorString;

    QVariant m_result;

    Status m_status;
    
    int m_redirects;
    
    QVariantList m_streams;
};

#endif // TVREQUEST_H
