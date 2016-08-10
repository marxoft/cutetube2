/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "plugincategorymodel.h"
#include "pluginmanager.h"
#include "resources.h"

PluginCategoryModel::PluginCategoryModel(QObject *parent) :
    SelectionModel(parent),
    m_request(0)
{
}

bool PluginCategoryModel::canFetchMore(const QModelIndex &) const {
    return (status() != ResourcesRequest::Loading) && (!m_next.isEmpty());
}

void PluginCategoryModel::fetchMore(const QModelIndex &) {
    if (!canFetchMore()) {
        return;
    }

    if (ResourcesRequest *r = request()) {
        r->list(Resources::CATEGORY, m_next);
        emit statusChanged(status());
    }
}

QString PluginCategoryModel::errorString() const {
    return m_request ? m_request->errorString() : QString();
}

QString PluginCategoryModel::service() const {
    return m_service;
}

void PluginCategoryModel::setService(const QString &s) {
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

ResourcesRequest::Status PluginCategoryModel::status() const {
    return m_request ? m_request->status() : ResourcesRequest::Null;
}

void PluginCategoryModel::list(const QString &resourceId) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourceId = resourceId;
    m_query = QString();

    if (ResourcesRequest *r = request()) {
        r->list(Resources::CATEGORY, resourceId);
        emit statusChanged(status());
    }
}

void PluginCategoryModel::search(const QString &query, const QString &order) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourceId = QString();
    m_query = query;
    m_order = order;

    if (ResourcesRequest *r = request()) {
        r->search(Resources::CATEGORY, query, order);
        emit statusChanged(status());
    }
}

void PluginCategoryModel::cancel() {
    if (m_request) {
        m_request->cancel();
    }
}

void PluginCategoryModel::reload() {
    clear();

    if (ResourcesRequest *r = request()) {
        if (m_query.isEmpty()) {
            r->list(Resources::CATEGORY, m_resourceId);
        }
        else {
            r->search(Resources::CATEGORY, m_query, m_order);
        }
        
        emit statusChanged(status());
    }
}

ResourcesRequest* PluginCategoryModel::request() {
    if (!m_request) {
        m_request = PluginManager::instance()->createRequestForService(service(), this);

        if (m_request) {
            connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
        }
    }

    return m_request;
}

void PluginCategoryModel::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        const QVariantMap result = m_request->result().toMap();
        m_next = QString();
        
        foreach (const QVariant &v, result.value("items").toList()) {
            const QVariantMap category = v.toMap();
            append(category.value("title").toString(), category);
        }
        
        m_next = result.value("next").toString();
    }
    
    emit statusChanged(status());
}
