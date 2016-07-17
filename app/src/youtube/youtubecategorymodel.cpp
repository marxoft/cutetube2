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

#include "youtubecategorymodel.h"
#include "youtube.h"

YouTubeCategoryModel::YouTubeCategoryModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QYouTube::ResourcesRequest(this))
{
    m_request->setApiKey(YouTube::apiKey());
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString YouTubeCategoryModel::errorString() const {
    return m_request->errorString();
}

QYouTube::ResourcesRequest::Status YouTubeCategoryModel::status() const {
    return m_request->status();
}

void YouTubeCategoryModel::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                                const QVariantMap &params) {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_part = part;
    m_filters = filters;
    m_params = params;
    m_request->list(resourcePath, part, filters, params);
    emit statusChanged(status());
}

void YouTubeCategoryModel::cancel() {
    m_request->cancel();
}

void YouTubeCategoryModel::reload() {
    if (status() == QYouTube::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_request->list(m_resourcePath, m_part, m_filters, m_params);
    emit statusChanged(status());
}

void YouTubeCategoryModel::onRequestFinished() {
    if (m_request->status() == QYouTube::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toMap().value("items").toList()) {
            const QVariantMap category = v.toMap();
            append(category.value("snippet").toMap().value("title").toString(), category.value("id").toString());
        }
    }
    
    emit statusChanged(status());
}
