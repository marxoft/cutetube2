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
#include <stdio.h>

static const int MAX_REDIRECTS = 8;

bool dateGreaterThan(QVariant &one, QVariant &two) {
    return one.toMap().value("_dt") > two.toMap().value("_dt");
}

Rss::Rss(QObject *parent) :
    QObject(parent),
    m_nam(new QNetworkAccessManager(this)),
    m_reply(0),
    m_redirects(0)
{
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
    if (m_reply) {
        m_reply->deleteLater();
        m_reply = 0;
    }
    
    m_reply = m_nam->get(QNetworkRequest(url));
    connect(m_reply, SIGNAL(finished()), this, SLOT(parseVideos()));
}

void Rss::parseVideos() {
    if (!m_reply) {
        printf(qPrintable(QString("{\"error\": \"%1\"}").arg(tr("Network error"))));
        QCoreApplication::exit(1);
        return;
    }
    
    if (m_reply->error() != QNetworkReply::NoError) {
        m_reply->deleteLater();
        m_reply = 0;
        printf(qPrintable(QString("{\"error\": \"%1: %2\"}").arg(tr("Network error")).arg(m_reply->errorString())));
        QCoreApplication::exit(1);
        return;
    }
    
    QVariant redirect = m_reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    
    if (!redirect.isNull()) {
        m_reply->deleteLater();
        m_reply = 0;
        
        if (m_redirects < MAX_REDIRECTS) {
            m_redirects++;
            listVideos(redirect.toString());
        }
        else {
            printf(qPrintable(QString("{\"error\": \"%1: %2\"}").arg(tr("Network error"))
                                                                .arg(tr("Maximum redirects reached"))));
            QCoreApplication::exit(1);
        }
        
        return;
    }
    
    QDomDocument doc;    
    
    if (!doc.setContent(m_reply->readAll(), true)) {
        m_reply->deleteLater();
        m_reply = 0;
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
        result["id"] = m_reply->url();
        result["largeThumbnailUrl"] = thumbnailUrl;
        result["streamUrl"] = item.firstChildElement("enclosure").attribute("url");
        result["thumbnailUrl"] = thumbnailUrl;
        result["title"] = item.firstChildElement("title").text();
        result["url"] = item.firstChildElement("link").text();
        result["username"] = item.firstChildElement("author").text();
        m_results << result;
    }
    
    m_reply->deleteLater();
    m_reply = 0;
    
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
