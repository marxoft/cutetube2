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

#include "javascriptresourcesrequest.h"
#include "logger.h"
#include "pluginsettings.h"
#include <QFile>
#include <QScriptEngine>

JavaScriptResourcesRequest::JavaScriptResourcesRequest(const QString &id, const QString &fileName, QObject *parent) :
    ResourcesRequest(parent),
    m_global(0),
    m_engine(0),
    m_fileName(fileName),
    m_id(id),
    m_evaluated(false)
{
}

QString JavaScriptResourcesRequest::fileName() const {
    return m_fileName;
}

QString JavaScriptResourcesRequest::id() const {
    return m_id;
}

QString JavaScriptResourcesRequest::errorString() const {
    return m_errorString;
}

void JavaScriptResourcesRequest::setErrorString(const QString &e) {
    m_errorString = e;
}

QVariant JavaScriptResourcesRequest::result() const {
    return m_result;
}

void JavaScriptResourcesRequest::setResult(const QVariant &r) {
    m_result = r;
}

ResourcesRequest::Status JavaScriptResourcesRequest::status() const {
    return m_status;
}

void JavaScriptResourcesRequest::setStatus(ResourcesRequest::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void JavaScriptResourcesRequest::initEngine() {
    if (m_evaluated) {
        return;
    }
    
    if (!m_engine) {
        m_engine = new QScriptEngine(this);
    }

    QFile file(fileName());
    
    if (file.open(QFile::ReadOnly)) {
        const QScriptValue result = m_engine->evaluate(file.readAll(), fileName());
        file.close();
        
        if (result.isError()) {
            Logger::log("JavaScriptResourcesRequest::initEngine(): Error evaluating JavaScript file: "
                        + result.toString());
            return;
        }
        
        Logger::log("JavaScriptResourcesRequest::initEngine(): JavaScript file evaluated OK");
        m_evaluated = true;
        m_global = new JavaScriptResourcesRequestGlobalObject(m_engine);
        
        connect(m_global, SIGNAL(error(QString)), this, SLOT(onRequestError(QString)));
        connect(m_global, SIGNAL(finished(QVariant)), this, SLOT(onRequestFinished(QVariant)));
        
        m_engine->installTranslatorFunctions();
        m_engine->globalObject().setProperty("settings", m_engine->newQObject(new PluginSettings(id(), m_engine)));
    }
    else {
        Logger::log("JavaScriptResourcesRequest::initEngine(): Error reading JavaScript file: "
                    + file.errorString());
    }
}

bool JavaScriptResourcesRequest::cancel() {
    if (!m_engine) {
        return false;
    }

    return m_engine->globalObject().property("cancel").call(QScriptValue()).toBool();
}

bool JavaScriptResourcesRequest::del(const QString &sourceType, const QString &sourceId, const QString &destinationType,
                                     const QString &destinationId) {
    if (status() == Loading) {
        return false;
    }
    
    initEngine();
    QScriptValue func = m_engine->globalObject().property("del");

    if (func.isFunction()) {
        const QScriptValue result = func.call(QScriptValue(), QScriptValueList() << sourceType << sourceId
                                                                                 << destinationType << destinationId);

        if (result.isError()) {
            const QString errorString = result.toString();
            Logger::log("JavaScriptResourcesRequest::del(). Error calling del(): " + errorString);
            setErrorString(errorString);
            setStatus(Failed);
            emit finished();
            return false;
        }

        if (result.toBool()) {
            setErrorString(QString());
            setStatus(Loading);
            return true;
        }
    }
    else {
        Logger::log("JavaScriptResourcesRequest::del(). del() function not defined");
        setErrorString(tr("del() function not defined"));
        setStatus(Failed);
        emit finished();
    }

    return false;
}

bool JavaScriptResourcesRequest::get(const QString &resourceType, const QString &resourceId) {
    if (status() == Loading) {
        return false;
    }
    
    initEngine();
    QScriptValue func = m_engine->globalObject().property("get");

    if (func.isFunction()) {
        const QScriptValue result = func.call(QScriptValue(), QScriptValueList() << resourceType << resourceId);

        if (result.isError()) {
            const QString errorString = result.toString();
            Logger::log("JavaScriptResourcesRequest::get(). Error calling get(): " + errorString);
            setErrorString(errorString);
            setStatus(Failed);
            emit finished();
            return false;
        }

        if (result.toBool()) {
            setErrorString(QString());
            setStatus(Loading);
            return true;
        }
    }
    else {
        Logger::log("JavaScriptResourcesRequest::get(). get() function not defined");
        setErrorString(tr("get() function not defined"));
        setStatus(Failed);
        emit finished();
    }

    return false;
}

bool JavaScriptResourcesRequest::insert(const QString &sourceType, const QString &sourceId,
                                        const QString &destinationType, const QString &destinationId) {
    if (status() == Loading) {
        return false;
    }
    
    initEngine();
    QScriptValue func = m_engine->globalObject().property("insert");

    if (func.isFunction()) {
        const QScriptValue result = func.call(QScriptValue(), QScriptValueList() << sourceType << sourceId
                                                                                 << destinationType << destinationId);

        if (result.isError()) {
            const QString errorString = result.toString();
            Logger::log("JavaScriptResourcesRequest::insert(). Error calling insert(): " + errorString);
            setErrorString(errorString);
            setStatus(Failed);
            emit finished();
            return false;
        }

        if (result.toBool()) {
            setErrorString(QString());
            setStatus(Loading);
            return true;
        }
    }
    else {
        Logger::log("JavaScriptResourcesRequest::insert(). insert() function not defined");
        setErrorString(tr("insert() function not defined"));
        setStatus(Failed);
        emit finished();
    }

    return false;
}

bool JavaScriptResourcesRequest::list(const QString &resourceType, const QString &resourceId) {
    if (status() == Loading) {
        return false;
    }
    
    initEngine();
    QScriptValue func = m_engine->globalObject().property("list");

    if (func.isFunction()) {
        const QScriptValue result = func.call(QScriptValue(), QScriptValueList() << resourceType << resourceId);

        if (result.isError()) {
            const QString errorString = result.toString();
            Logger::log("JavaScriptResourcesRequest::list(). Error calling list(): " + errorString);
            setErrorString(errorString);
            setStatus(Failed);
            emit finished();
            return false;
        }

        if (result.toBool()) {
            setErrorString(QString());
            setStatus(Loading);
            return true;
        }
    }
    else {
        Logger::log("JavaScriptResourcesRequest::list(). list() function not defined");
        setErrorString(tr("list() function not defined"));
        setStatus(Failed);
        emit finished();
    }

    return false;
}

bool JavaScriptResourcesRequest::search(const QString &resourceType, const QString &query, const QString &order) {
    if (status() == Loading) {
        return false;
    }
    
    initEngine();
    QScriptValue func = m_engine->globalObject().property("search");

    if (func.isFunction()) {
        const QScriptValue result = func.call(QScriptValue(), QScriptValueList() << resourceType << query << order);

        if (result.isError()) {
            const QString errorString = result.toString();
            Logger::log("JavaScriptResourcesRequest::search(). Error calling search(): " + errorString);
            setErrorString(errorString);
            setStatus(Failed);
            emit finished();
            return false;
        }

        if (result.toBool()) {
            setErrorString(QString());
            setStatus(Loading);
            return true;
        }
    }
    else {
        Logger::log("JavaScriptResourcesRequest::search(). search() function not defined");
        setErrorString(tr("search() function not defined"));
        setStatus(Failed);
        emit finished();
    }

    return false;
}

void JavaScriptResourcesRequest::onRequestError(const QString &errorString) {
    Logger::log("JavaScriptResourcesRequest::onRequestError(): " + errorString);
    setErrorString(errorString);
    setResult(QVariant());
    setStatus(Failed);
    emit finished();
}

void JavaScriptResourcesRequest::onRequestFinished(const QVariant &result) {
    Logger::log("JavaScriptResourcesRequest::onRequestFinished()");
    setErrorString(QString());
    setResult(result);
    setStatus(Ready);
    emit finished();
}
