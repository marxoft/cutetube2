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

#include "pluginsubtitlemodel.h"
#include "pluginmanager.h"
#include "resources.h"

PluginSubtitleModel::PluginSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(0)
{
}

QString PluginSubtitleModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginSubtitleModel::service() const {
    return m_service;
}

void PluginSubtitleModel::setService(const QString &s) {
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

ResourcesRequest::Status PluginSubtitleModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginSubtitleModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourceId = resourceId;

    if (ResourcesRequest *r = request()) {
        r->list(Resources::SUBTITLE, resourceId);
        emit statusChanged(status());
    }
}

void PluginSubtitleModel::cancel() {
    if (m_request) {
        m_request->cancel();
    }
}

void PluginSubtitleModel::reload() {
    clear();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::SUBTITLE, m_resourceId);
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginSubtitleModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginSubtitleModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toMap().value("items").toList()) {
            const QVariantMap sub = v.toMap();
            append(sub.value("title").toString(), sub);
        }
    }
    
    emit statusChanged(status());
}
