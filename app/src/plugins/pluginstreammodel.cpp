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

#include "pluginstreammodel.h"
#include "pluginmanager.h"
#include "resources.h"

PluginStreamModel::PluginStreamModel(QObject *parent) :
    SelectionModel(parent),
    m_request(0)
{
}

QString PluginStreamModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginStreamModel::service() const {
    return m_service;
}

void PluginStreamModel::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();

        clear();

        if (m_request) {
            m_request->deleteLater();
            m_request = 0;
        }
    }
}

ResourcesRequest::Status PluginStreamModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginStreamModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourceId = resourceId;

    if (ResourcesRequest *r = request()) {
        r->list(Resources::STREAM, resourceId);
        emit statusChanged(status());
    }
}

void PluginStreamModel::cancel() {
    if (m_request) {
        m_request->cancel();
    }
}

void PluginStreamModel::reload() {
    clear();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::STREAM, m_resourceId);
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginStreamModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginStreamModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toMap().value("items").toList()) {
            const QVariantMap stream = v.toMap();
            const int height = stream.value("height").toInt();
            
            if (height > 0) {
                append(QString("%1p %2").arg(height).arg(stream.value("description").toString()), stream);
            }
            else {
                append(tr("Unknown resolution %1").arg(stream.value("description").toString()), stream);
            }
        }
    }
    
    emit statusChanged(status());
}
