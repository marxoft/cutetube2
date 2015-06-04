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

#include "pluginsubtitlemodel.h"
#include "resources.h"

PluginSubtitleModel::PluginSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString PluginSubtitleModel::service() const {
    return m_request->service();
}

void PluginSubtitleModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginSubtitleModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginSubtitleModel::status() const {
    return m_request->status();
}

void PluginSubtitleModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->list(Resources::SUBTITLE, id);
    emit statusChanged(status());
}

void PluginSubtitleModel::cancel() {
    m_request->cancel();
}

void PluginSubtitleModel::reload() {
    clear();
    m_request->list(Resources::SUBTITLE, m_id);
    emit statusChanged(status());
}

void PluginSubtitleModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        foreach (QVariant v, m_request->result().toMap().value("items").toList()) {
            QVariantMap sub = v.toMap();
            append(sub.value("title").toString(), sub);
        }
    }
    
    emit statusChanged(status());
}
