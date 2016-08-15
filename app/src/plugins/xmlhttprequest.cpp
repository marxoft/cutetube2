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

#include "xmlhttprequest.h"
#include "definitions.h"
#include "logger.h"
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkReply>

XMLHttpRequest::XMLHttpRequest(QObject *parent) :
    QObject(parent),
    m_nam(0),
    m_reply(0),
    m_readyState(UNSENT),
    m_status(0),
    m_redirects(0)
{
}

XMLHttpRequest::XMLHttpRequest(QNetworkAccessManager *manager, QObject *parent) :
    QObject(parent),
    m_nam(manager),
    m_reply(0),
    m_readyState(UNSENT),
    m_status(0),
    m_redirects(0)
{
}

QNetworkAccessManager* XMLHttpRequest::networkAccessManager() {
    return m_nam ? m_nam : m_nam = new QNetworkAccessManager(this);
}

int XMLHttpRequest::readyState() const {
    return m_readyState;
}

void XMLHttpRequest::setReadyState(int state) {
    Logger::log("XMLHttpRequest::setReadyState(): readyState: " + QString::number(state), Logger::MediumVerbosity);
    
    if (state != readyState()) {
        m_readyState = state;
        m_onReadyStateChange.call(QScriptValue());
    }
}

QString XMLHttpRequest::responseText() const {
    return readyState() == DONE ? QString::fromUtf8(m_response) : QString();
}

QString XMLHttpRequest::responseXML() const {
    return readyState() == DONE ? QString::fromUtf8(m_response) : QString();
}

int XMLHttpRequest::status() const {
    return m_status;
}

void XMLHttpRequest::setStatus(int s) {
    m_status = s;
}

QString XMLHttpRequest::statusText() const {
    return m_statusText;
}

void XMLHttpRequest::setStatusText(const QString &text) {
    m_statusText = text;
}

QScriptValue XMLHttpRequest::onReadyStateChange() const {
    return m_onReadyStateChange;
}

void XMLHttpRequest::setOnReadyStateChange(const QScriptValue &function) {
    m_onReadyStateChange = function;
}

void XMLHttpRequest::setRequestHeader(const QString &name, const QString &value) {
    m_request.setRawHeader(name.toUtf8(), value.toUtf8());
}

QString XMLHttpRequest::getResponseHeader(const QString &name) const {
    return m_responseHeaders.value(name);
}

QString XMLHttpRequest::getAllResponseHeaders() const {
    if (m_responseHeaders.isEmpty()) {
        return QString();
    }

    QString headerString;
    QMapIterator<QString, QString> iterator(m_responseHeaders);

    while (iterator.hasNext()) {
        iterator.next();
        headerString.append(iterator.key());
        headerString.append(": ");
        headerString.append(iterator.value());
        headerString.append("\n");
    }

    return headerString;
}

void XMLHttpRequest::open(const QString &method, const QString &url, const QString &username, const QString &password) {
    Logger::log(QString("XMLHttpRequest::open(). Method: %1, URL: %2, Username: %3, Password: %4")
                       .arg(method).arg(url).arg(username).arg(password), Logger::MediumVerbosity);
    
    switch (readyState()) {
    case OPENED:
    case HEADERS_RECEIVED:
    case LOADING:
        return;
    default:
        break;
    }
    
    m_method = method.toUtf8();
    m_request.setUrl(QUrl::fromUserInput(url));

    if ((!username.isEmpty()) || (!password.isEmpty())) {
        m_request.setRawHeader("Authorization", "Basic " + QString("%1:%2").arg(username).arg(password).toUtf8()
                                                                           .toBase64());
    }
    
    setReadyState(OPENED);
}

void XMLHttpRequest::send(const QString &body) {
    Logger::log("XMLHttpRequest::send(): Body: " + body, Logger::MediumVerbosity);
    
    switch (readyState()) {
    case HEADERS_RECEIVED:
    case LOADING:
        return;
    default:
        break;
    }

    m_redirects = 0;
    QBuffer *buffer = new QBuffer;
    buffer->setData(body.toUtf8());
    buffer->open(QBuffer::ReadOnly);
    m_reply = networkAccessManager()->sendCustomRequest(m_request, m_method, buffer);
    buffer->setParent(m_reply);
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(onReplyMetaDataChanged()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void XMLHttpRequest::abort() {
    Logger::log("XMLHttpRequest::abort()", Logger::MediumVerbosity);
    
    if ((m_reply) && (m_reply->isRunning())) {
        m_reply->abort();
    }
}

void XMLHttpRequest::followRedirect(const QUrl &url) {
    Logger::log("XMLHttpRequest::followRedirect(): URL: " + url.toString(), Logger::MediumVerbosity);
    m_redirects++;
    m_response = QByteArray();
    QNetworkRequest request(m_request);
    request.setUrl(url);
    m_reply = networkAccessManager()->get(request);
    setReadyState(OPENED);
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(onReplyMetaDataChanged()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void XMLHttpRequest::reset() {
    setReadyState(UNSENT);
    setStatus(0);
    setStatusText(QString());
    m_request = QNetworkRequest();
    m_response = QByteArray();
    m_responseHeaders.clear();
}

void XMLHttpRequest::onReplyMetaDataChanged() {
    if (readyState() > OPENED) {
        return;
    }

    setReadyState(HEADERS_RECEIVED);    

    foreach (const QByteArray &header, m_reply->rawHeaderList()) {
        m_responseHeaders[QString::fromUtf8(header)] = QString::fromUtf8(m_reply->rawHeader(header));
    }

    setReadyState(LOADING);
}

void XMLHttpRequest::onReplyReadyRead() {
    m_response += m_reply->readAll();
}

void XMLHttpRequest::onReplyFinished() {
    const QString redirect = QString::fromUtf8(m_reply->rawHeader("Location"));
    
    if (!redirect.isEmpty()) {
        Logger::log("XMLHttpRequest::onReplyFinished(): Redirect: " + redirect, Logger::MediumVerbosity);
        
        if (m_redirects < MAX_REDIRECTS) {
            QUrl url(redirect);
            
            if (url.scheme().isEmpty()) {
                url.setScheme(m_reply->url().scheme());
            }
            
            if (url.authority().isEmpty()) {
                url.setAuthority(m_reply->url().authority());
            }
            
            m_reply->deleteLater();
            m_reply = 0;
            followRedirect(url);
            return;
        }
    }
    
    if (m_reply->bytesAvailable() > 0) {
        m_response += m_reply->readAll();
    }
    
    setStatus(m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
    setStatusText(m_reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString());
    setReadyState(DONE);
    m_reply->deleteLater();
    m_reply = 0;
}
