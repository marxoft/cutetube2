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
#include <iostream>

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
        std::cout << qPrintable(QString("{\"error\": \"%1\"}").arg(tr("No feed URLs specified")));
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
        std::cout << qPrintable(QString("{\"error\": \"%1\"}").arg(tr("Network error")));
        QCoreApplication::exit(1);
        return;
    }
    
    if (reply->error() != QNetworkReply::NoError) {
        reply->deleteLater();
        std::cout << qPrintable(QString("{\"error\": \"%1: %2\"}").arg(tr("Network error")).arg(reply->errorString()));
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
            std::cout << qPrintable(QString("{\"error\": \"%1: %2\"}").arg(tr("Network error"))
                                                                .arg(tr("Maximum redirects reached")));
            QCoreApplication::exit(1);
        }
        
        return;
    }
    
    QDomDocument doc;    
    
    if (!doc.setContent(reply->readAll(), true)) {
        reply->deleteLater();
        std::cout << qPrintable(QString("{\"error\": \"%1\"}").arg(tr("Unable to parse XML")));
        QCoreApplication::exit(1);
        return;
    }
    
    QDomElement docElem = doc.documentElement();
    QDomNodeList items = docElem.elementsByTagName("item");
    QDomNode channelElem = docElem.firstChildElement("channel");
    QString thumbnailUrl = channelElem.firstChildElement("image").attribute("href");
    QString genre = channelElem.firstChildElement("category").attribute("text");
    
    for (int i = 0; i < items.size(); i++) {
        QDomElement item = items.at(i).toElement();
        QDateTime dt = QDateTime::fromString(item.firstChildElement("pubDate").text().section(' ', 0, -2),
                                               "ddd, dd MMM yyyy hh:mm:ss");
        QString streamUrl = item.firstChildElement("enclosure").attribute("url");
        
        QVariantMap result;
        result["_dt"] = dt;
        result["artist"] = item.firstChildElement("author").text();
        result["date"] = dt.toString("dd MMM yyyy");
        result["description"] = item.firstChildElement("description").text();
        result["duration"] = item.firstChildElement("duration").text();
        result["format"] = streamUrl.mid(streamUrl.lastIndexOf('.') + 1).toUpper();
        result["genre"] = genre;
        result["id"] = reply->url();
        result["largeThumbnailUrl"] = thumbnailUrl;
        result["streamUrl"] = streamUrl;
        result["thumbnailUrl"] = thumbnailUrl;
        result["title"] = item.firstChildElement("title").text();
        result["url"] = item.firstChildElement("link").text();
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
    
    std::cout << QByteArray("{\"items\": " + QtJson::Json::serialize(m_results) + "}").constData();
    QCoreApplication::quit();
}
