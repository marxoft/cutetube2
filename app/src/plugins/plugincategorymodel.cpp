/*
 * Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
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

#include "plugincategorymodel.h"
#include "resources.h"

PluginCategoryModel::PluginCategoryModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(serviceChanged()), this, SIGNAL(serviceChanged()));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

bool PluginCategoryModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginCategoryModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }
    
    m_request->list(Resources::CATEGORY, m_next);
    emit statusChanged(status());
}

QString PluginCategoryModel::service() const {
    return m_request->service();
}

void PluginCategoryModel::setService(const QString &s) {
    m_request->setService(s);
}

QString PluginCategoryModel::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginCategoryModel::status() const {
    return m_request->status();
}

void PluginCategoryModel::list(const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_next = QString();
    m_request->list(Resources::CATEGORY, id);
    emit statusChanged(status());
}

void PluginCategoryModel::cancel() {
    m_request->cancel();
}

void PluginCategoryModel::reload() {
    clear();
    m_next = QString();
    m_request->list(Resources::CATEGORY, m_id);
    emit statusChanged(status());
}

void PluginCategoryModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        QVariantMap result = m_request->result().toMap();
        m_next = QString();
        
        foreach (QVariant v, result.value("items").toList()) {
            QVariantMap category = v.toMap();
            append(category.value("title").toString(), category.value("id").toString());
        }
        
        m_next = result.value("next").toString();
    }
    
    emit statusChanged(status());
}
