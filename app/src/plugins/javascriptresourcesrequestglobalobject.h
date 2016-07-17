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

#ifndef JAVASCRIPTRESOURCESREQUESTGLOBALOBJECT_H
#define JAVASCRIPTRESOURCESREQUESTGLOBALOBJECT_H

#include <QObject>
#include <QPointer>
#include <QScriptEngine>

class QNetworkAccessManager;

class JavaScriptResourcesRequestGlobalObject : public QObject
{
    Q_OBJECT

public:
    explicit JavaScriptResourcesRequestGlobalObject(QScriptEngine *engine);

public Q_SLOTS:
    QString atob(const QString &ascii) const;
    QString btoa(const QString &binary) const;

    void clearInterval(int timerId);
    void clearTimeout(int timerId);

    void setInterval(const QScriptValue &function, int msecs);
    void setTimeout(const QScriptValue &function, int msecs);

Q_SIGNALS:
    void error(const QString &errorString);
    void finished(const QVariant &result);

private:
    static QScriptValue newXMLHttpRequest(QScriptContext *context, QScriptEngine *engine);
    
    QNetworkAccessManager* networkAccessManager();
        
    bool callFunction(QScriptValue function) const;
        
    virtual void timerEvent(QTimerEvent *event);
    
    QNetworkAccessManager *m_nam;

    QPointer<QScriptEngine> m_engine;
    
    QHash<int, QScriptValue> m_intervals;
    QHash<int, QScriptValue> m_timeouts;
};

#endif // JAVASCRIPTRESOURCESREQUESTGLOBALOBJECT_H
