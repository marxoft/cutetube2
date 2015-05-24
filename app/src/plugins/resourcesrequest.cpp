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

#include "resourcesrequest.h"
#include "json.h"
#include "resourcesplugins.h"
#include <QProcess>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

ResourcesRequest::ResourcesRequest(QObject *parent) :
    QObject(parent),
    m_process(new QProcess(this)),
    m_status(Null),
    m_error(NoError)
{
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onProcessFinished(int)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError()));
}

QString ResourcesRequest::service() const {
    return m_service;
}

void ResourcesRequest::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "ResourcesRequest::setService" << s;
#endif
}

ResourcesRequest::Status ResourcesRequest::status() const {
    return m_status;
}

void ResourcesRequest::setStatus(Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
#ifdef CUTETUBE_DEBUG
    qDebug() << "ResourcesRequest::setStatus" << s;
#endif
}

QVariant ResourcesRequest::result() const {
    return m_result;
}

void ResourcesRequest::setResult(const QVariant &r) {
    m_result = r;
#ifdef CUTETUBE_DEBUG
    qDebug() << "ResourcesRequest::setResult" << r;
#endif
}

ResourcesRequest::Error ResourcesRequest::error() const {
    return m_error;
}

void ResourcesRequest::setError(Error e) {
    m_error = e;
#ifdef CUTETUBE_DEBUG
    qDebug() << "ResourcesRequest::setError" << e;
#endif
}

QString ResourcesRequest::errorString() const {
    return m_errorString;
}

void ResourcesRequest::setErrorString(const QString &es) {
    m_errorString = es;
#ifdef CUTETUBE_DEBUG
    qDebug() << "ResourcesRequest::setErrorString" << es;
#endif
}

void ResourcesRequest::list(const QString &resourceType, const QString &id) {
    if (status() == Loading) {
        return;
    }
    
    ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromName(service());
    
    if (plugin.command.isEmpty()) {
        setStatus(Failed);
        setError(PluginError);
        setErrorString(tr("No plugin found for %1").arg(service()));
        emit finished();
        return;
    }
    
    setStatus(Loading);
    QStringList args = QStringList() << "-m" << "list" << "-r" << resourceType;
    
    if (!id.isEmpty()) {
        args << "-i" << id;
    }
    
    start(plugin.command, args);
}

void ResourcesRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if (status() == Loading) {
        return;
    }
        
    ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromName(service());
    
    if (plugin.command.isEmpty()) {
        setStatus(Failed);
        setError(PluginError);
        setErrorString(tr("No plugin found for %1").arg(service()));
        emit finished();
        return;
    }
    
    setStatus(Loading);
    start(plugin.command, QStringList() << "-m" << "search" << "-r" << resourceType << "-q" << query << "-o" << order);
}

void ResourcesRequest::get(const QString &resourceType, const QString &id) {
    if (status() == Loading) {
        return;
    }
    
    ResourcesPlugin plugin = ResourcesPlugins::instance()->getPluginFromName(service());
    
    if (plugin.command.isEmpty()) {
        setStatus(Failed);
        setError(PluginError);
        setErrorString(tr("No plugin found for %1").arg(service()));
        emit finished();
        return;
    }
    
    setStatus(Loading);
    start(plugin.command, QStringList() << "-m" << "get" << "-r" << resourceType << "-i" << id);
}

void ResourcesRequest::start(const QString &program, const QStringList &args) {
#ifdef CUTETUBE_DEBUG
    qDebug() << "ResourcesRequest::start" << program << args;
#endif
    m_process->start(program, args);
}

void ResourcesRequest::cancel() {
    m_process->kill();
}

void ResourcesRequest::onProcessFinished(int exitCode) {
    bool ok;
    setResult(QtJson::Json::parse(QString::fromUtf8(m_process->readAllStandardOutput()), ok));
        
    if (exitCode == 0) {
        if (ok) {
            setStatus(Ready);
            setError(NoError);
            setErrorString(QString());
        }
        else {
            setStatus(Failed);
            setError(ParseError);
            setErrorString(tr("Unable to parse response"));
        }
    }
    else {
        setStatus(Failed);
        setError(ProcessError);
        
        if (result().type() == QVariant::Map) {
            QVariantMap map = result().toMap();
            
            if (map.contains("error")) {
                setErrorString(map.value("error").toString());
            }
            else {
                setErrorString(m_process->errorString());
            }
        }
        else {
            setErrorString(m_process->errorString());
        }
    }
    
    emit finished();
}

void ResourcesRequest::onProcessError() {
    setStatus(Failed);
    setError(ProcessError);
    setErrorString(m_process->errorString());
    emit finished();
}
