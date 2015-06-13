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

#include "rss.h"
#include "json.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QDateTime>
#include <stdio.h>

static const int MAX_REDIRECTS = 8;

bool dateGreaterThan(QVariant &one, QVariant &two) {
    return one.toMap().value("_dt").toDateTime() > two.toMap().value("_dt").toDateTime();
}

Rss::Rss(QObject *parent) :
    QObject(parent),
    m_nam(new QNetworkAccessManager(this)),
    m_redirects(0)
{
    connect(m_nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(parseVideos(QNetworkReply*)));
}

void Rss::listVideos(const QStringList &urls) {
    m_urls = urls;
    
    if (m_urls.isEmpty()) {
        printf(qPrintable(QString("{\"error\": \"%1\"}").arg(tr("No feed URLs specified"))));
        QCoreApplication::exit(1);
        return;
    }
    
    listVideos(m_urls.takeFirst());
}

void Rss::listVideos(const QString &url) {    
    m_redirects = 0;
    m_nam->get(QNetworkRequest(url));
}

void Rss::followRedirect(const QUrl &url) {
    m_redirects++;
    m_nam->get(QNetworkRequest(url));
}

void Rss::parseVideos(QNetworkReply *reply) {
    if (!reply) {
        printf(qPrintable(QString("{\"error\": \"%1\"}").arg(tr("Network error"))));
        QCoreApplication::exit(1);
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        printf(qPrintable(QString("{\"error\": \"%1: %2\"}").arg(tr("Network error")).arg(reply->errorString())));
        QCoreApplication::exit(1);
        return;
    }
    
    QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    
    if (!redirect.isNull()) {
        reply->deleteLater();
        
        if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect.toString());
        }
        else {
            printf(qPrintable(QString("{\"error\": \"%1: %2\"}").arg(tr("Network error"))
                                                                .arg(tr("Maximum redirects reached"))));
            QCoreApplication::exit(1);
        }
        
        return;
    }
    
    QDomDocument doc;    
    
    if (!doc.setContent(reply->readAll(), true)) {
        reply->deleteLater();
        printf(qPrintable(QString("{\"error\": \"%1\"}").arg(tr("Unable to parse XML"))));
        QCoreApplication::exit(1);
        return;
    }
    
    QDomElement docElem = doc.documentElement();
    QDomNodeList items = docElem.elementsByTagName("item");
    QString thumbnailUrl =  docElem.firstChildElement("channel").firstChildElement("image").attribute("href");
    
    for (int i = 0; i < items.size(); i++) {
        QDomElement item = items.at(i).toElement();
        QDateTime dt = QDateTime::fromString(item.firstChildElement("pubDate").text().section(' ', 0, -2),
                                               "ddd, dd MMM yyyy hh:mm:ss");
        QVariantMap result;
        result["_dt"] = dt;
        result["date"] = dt.toString("dd MMM yyyy");
        result["description"] = item.firstChildElement("description").text();
        result["duration"] = item.firstChildElement("duration").text();
        result["id"] = reply->url();
        result["largeThumbnailUrl"] = thumbnailUrl;
        result["streamUrl"] = item.firstChildElement("enclosure").attribute("url");
        result["thumbnailUrl"] = thumbnailUrl;
        result["title"] = item.firstChildElement("title").text();
        result["url"] = item.firstChildElement("link").text();
        result["username"] = item.firstChildElement("author").text();
        m_results << result;
    }
    
    reply->deleteLater();
    
    if (m_urls.isEmpty()) {
        printResult();
    }
    else {
        listVideos(m_urls.takeFirst());
    }
}

void Rss::printResult() {
    if (!m_results.isEmpty()) {
        qSort(m_results.begin(), m_results.end(), dateGreaterThan);
    }
    
    printf(QByteArray("{\"items\": " + QtJson::Json::serialize(m_results) + "}").constData());
    QCoreApplication::quit();
}
