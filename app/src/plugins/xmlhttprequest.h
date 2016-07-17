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

#ifndef XMLHTTPREQUEST_H
#define XMLHTTPREQUEST_H

#include <QObject>
#include <QNetworkRequest>
#include <QPointer>
#include <QScriptValue>

class QNetworkAccessManager;
class QNetworkReply;

class XMLHttpRequest : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int readyState READ readyState)
    Q_PROPERTY(QString responseText READ responseText)
    Q_PROPERTY(QString responseXML READ responseXML)
    Q_PROPERTY(int status READ status)
    Q_PROPERTY(QString statusText READ statusText)
    Q_PROPERTY(QScriptValue onreadystatechange READ onReadyStateChange WRITE setOnReadyStateChange)

    Q_ENUMS(ReadyState)

public:
    enum ReadyState {
        UNSENT = 0,
        OPENED,
        HEADERS_RECEIVED,
        LOADING,
        DONE
    };
    
    explicit XMLHttpRequest(QObject *parent = 0);
    explicit XMLHttpRequest(QNetworkAccessManager *manager, QObject *parent = 0);

    int readyState() const;

    QString responseText() const;
    QString responseXML() const;

    int status() const;
    QString statusText() const;

    QScriptValue onReadyStateChange() const;
    void setOnReadyStateChange(const QScriptValue &function);
    
public Q_SLOTS:
    void setRequestHeader(const QString &name, const QString &value);

    QString getResponseHeader(const QString &name) const;
    QString getAllResponseHeaders() const;
    
    void open(const QString &method, const QString &url, const QString &username = QString(),
              const QString &password = QString());
    void send(const QString &body = QString());
    void abort();    

private Q_SLOTS:
    void onReplyMetaDataChanged();
    void onReplyReadyRead();
    void onReplyFinished();

private:
    QNetworkAccessManager* networkAccessManager();
    
    void setReadyState(int state);
    
    void setStatus(int s);
    void setStatusText(const QString &text);

    void followRedirect(const QUrl &redirect);

    void reset();
    
    QPointer<QNetworkAccessManager> m_nam;
    QNetworkReply *m_reply;

    QNetworkRequest m_request;

    int m_readyState;
    int m_status;
    int m_redirects;

    QString m_statusText;
    
    QByteArray m_method;
    QByteArray m_response;

    QMap<QString, QString> m_responseHeaders;

    QScriptValue m_onReadyStateChange;
};

Q_DECLARE_METATYPE(XMLHttpRequest*)

#endif // XMLHTTPREQUEST_H
