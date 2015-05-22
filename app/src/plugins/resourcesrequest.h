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
 
#ifndef RESOURCESREQUEST_H
#define RESOURCESREQUEST_H

#include <QObject>
#include <QString>
#include <QVariant>

class QProcess;

class ResourcesRequest : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QVariant result READ result NOTIFY finished)
    Q_PROPERTY(Error error READ error NOTIFY finished)
    Q_PROPERTY(QString errorString READ errorString NOTIFY finished)

    Q_ENUMS(Status Error)
        
public:
    enum Status {
        Null = 0,
        Loading,
        Canceled,
        Ready,
        Failed
    };
    
    enum Error {
        NoError = 0,
        ProcessError,
        PluginError,
        ParseError,
        UnknownError
    };
        
    explicit ResourcesRequest(QObject *parent = 0);
    
    QString service() const;
    void setService(const QString &s);
    
    Status status() const;
    
    QVariant result() const;
    
    Error error() const;
    QString errorString() const;
    
    Q_INVOKABLE void list(const QString &resourceType, const QString &id = QString());
    
    Q_INVOKABLE void search(const QString &resourceType, const QString &query, const QString &order);
    
    Q_INVOKABLE void get(const QString &resourceType, const QString &id);

public Q_SLOTS:
    void cancel();
    
private:
    void start(const QString &program, const QStringList &args);
    
    void setStatus(Status s);
    
    void setResult(const QVariant &r);
    
    void setError(Error e);
    
    void setErrorString(const QString &es);
    
private Q_SLOTS:
    void onProcessFinished(int exitCode);
    void onProcessError();
    
Q_SIGNALS:
    void serviceChanged();
    void statusChanged(Status s);
    void finished();
    
private:
    QProcess *m_process;
        
    QString m_service;
    
    Status m_status;
    
    QVariant m_result;
    
    Error m_error;
    QString m_errorString;
};

#endif // RESOURCESREQUEST_H
