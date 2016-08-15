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

#include "externalresourcesrequest.h"
#include "json.h"
#include "logger.h"
#include <QProcess>

ExternalResourcesRequest::ExternalResourcesRequest(const QString &id, const QString &fileName, QObject *parent) :
    ResourcesRequest(parent),
    m_process(0),
    m_fileName(fileName),
    m_id(id)
{
}

QString ExternalResourcesRequest::fileName() const {
    return m_fileName;
}

QString ExternalResourcesRequest::id() const {
    return m_id;
}

QString ExternalResourcesRequest::errorString() const {
    return m_errorString;
}

void ExternalResourcesRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant ExternalResourcesRequest::result() const {
    return m_result;
}

void ExternalResourcesRequest::setResult(const QVariant &r) {
    m_result = r;
}

ResourcesRequest::Status ExternalResourcesRequest::status() const {
    return m_status;
}

void ExternalResourcesRequest::setStatus(ResourcesRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QProcess* ExternalResourcesRequest::process() {
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onRequestError()));
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onRequestFinished(int)));
    }
    
    return m_process;
}

bool ExternalResourcesRequest::cancel() {
    return false;
}

bool ExternalResourcesRequest::del(const QString &sourceType, const QString &sourceId, const QString &destinationType,
                                   const QString &destinationId) {
    if (status() == Loading) {
        return false;
    }
    
    setStatus(Loading);
    const QString command = QString("\"%1\" del \"%2\" \"%3\" \"%4\" \"%5\"").arg(fileName()).arg(sourceType)
                                                                             .arg(sourceId).arg(destinationType)
                                                                             .arg(destinationId);
    QProcess *pr = process();
    pr->start(command);
    
    switch (pr->state()) {
    case QProcess::Starting:
    case QProcess::Running:
        Logger::log("ExternalResourcesRequest::del(). Invoking command: " + command, Logger::MediumVerbosity);
        return true;
    default:
        Logger::log("ExternalResourcesRequest::del(). Error invoking command: " + command);
        return false;
    }
}

bool ExternalResourcesRequest::get(const QString &resourceType, const QString &resourceId) {
    if (status() == Loading) {
        return false;
    }
    
    setStatus(Loading);
    const QString command = QString("\"%1\" get \"%2\" \"%3\"").arg(fileName()).arg(resourceType).arg(resourceId);
    QProcess *pr = process();
    pr->start(command);
    
    switch (pr->state()) {
    case QProcess::Starting:
    case QProcess::Running:
        Logger::log("ExternalResourcesRequest::get(). Invoking command: " + command, Logger::MediumVerbosity);
        return true;
    default:
        Logger::log("ExternalResourcesRequest::get(). Error invoking command: " + command);
        return false;
    };
}

bool ExternalResourcesRequest::insert(const QString &sourceType, const QString &sourceId,
                                      const QString &destinationType, const QString &destinationId) {
    if (status() == Loading) {
        return false;
    }
    
    setStatus(Loading);
    const QString command = QString("\"%1\" insert \"%2\" \"%3\" \"%4\" \"%5\"").arg(fileName()).arg(sourceType)
                                                                                .arg(sourceId).arg(destinationType)
                                                                                .arg(destinationId);
    QProcess *pr = process();
    pr->start(command);
    
    switch (pr->state()) {
    case QProcess::Starting:
    case QProcess::Running:
        Logger::log("ExternalResourcesRequest::insert(). Invoking command: " + command, Logger::MediumVerbosity);
        return true;
    default:
        Logger::log("ExternalResourcesRequest::insert(). Error invoking command: " + command);
        return false;
    }
}

bool ExternalResourcesRequest::list(const QString &resourceType, const QString &resourceId) {
    if (status() == Loading) {
        return false;
    }
    
    setStatus(Loading);
    const QString command = QString("\"%1\" list \"%2\" \"%3\"").arg(fileName()).arg(resourceType).arg(resourceId);
    QProcess *pr = process();
    pr->start(command);
    
    switch (pr->state()) {
    case QProcess::Starting:
    case QProcess::Running:
        Logger::log("ExternalResourcesRequest::list(). Invoking command: " + command, Logger::MediumVerbosity);
        return true;
    default:
        Logger::log("ExternalResourcesRequest::list(). Error invoking command: " + command);
        return false;
    }
}

bool ExternalResourcesRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if (status() == Loading) {
        return false;
    }
    
    setStatus(Loading);
    const QString command = QString("\"%1\" search \"%2\" \"%3\" \"%4\"").arg(fileName()).arg(resourceType).arg(query)
                                                                         .arg(order);
    QProcess *pr = process();
    pr->start(command);
    
    switch (pr->state()) {
    case QProcess::Starting:
    case QProcess::Running:
        Logger::log("ExternalResourcesRequest::search(). Invoking command: " + command, Logger::MediumVerbosity);
        return true;
    default:
        Logger::log("ExternalResourcesRequest::search(). Error invoking command: " + command);
        return false;
    }
}

void ExternalResourcesRequest::onRequestError() {
    const QString errorString = m_process->errorString();
    Logger::log("ExternalResourcesRequest::onRequestError(): " + errorString);
    setErrorString(errorString);
    setResult(QVariant());
    setStatus(Failed);
    emit finished();
}

void ExternalResourcesRequest::onRequestFinished(int exitCode) {
    const QVariant result = QtJson::Json::parse(QString::fromUtf8(m_process->readAllStandardOutput()));
    setResult(result);
    
    if (exitCode == 0) {
        Logger::log("ExternalResourcesRequest::onRequestFinished(). Exit code 0", Logger::MediumVerbosity);
        setErrorString(QString());
        setStatus(Ready);
    }
    else {
        if (result.type() == QVariant::Map) {
            const QString errorString = result.toMap().value("error").toString();
            
            if (!errorString.isEmpty()) {
                setErrorString(errorString);
            }
            else {
                setErrorString(tr("Unknown error"));
            }
        }
        else {
            setErrorString(m_process->errorString());
        }
        
        Logger::log(QString("ExternalResourcesRequest::onRequestFinished(). Exit code: %1, Error: %2")
                           .arg(exitCode).arg(errorString()));
        setStatus(Failed);
    }
    
    emit finished();
}
