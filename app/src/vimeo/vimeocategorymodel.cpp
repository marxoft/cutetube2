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

#include "vimeocategorymodel.h"
#include "vimeo.h"

VimeoCategoryModel::VimeoCategoryModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QVimeo::ResourcesRequest(this))
{
    m_request->setClientId(Vimeo::instance()->clientId());
    m_request->setClientSecret(Vimeo::instance()->clientSecret());
    m_request->setAccessToken(Vimeo::instance()->accessToken());
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString VimeoCategoryModel::errorString() const {
    return m_request->errorString();
}

QVimeo::ResourcesRequest::Status VimeoCategoryModel::status() const {
    return m_request->status();
}

void VimeoCategoryModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters);
    emit statusChanged(status());
}

void VimeoCategoryModel::cancel() {
    m_request->cancel();
}

void VimeoCategoryModel::reload() {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void VimeoCategoryModel::onRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        foreach (QVariant v, m_request->result().toMap().value("list").toList()) {
            QVariantMap category = v.toMap();
            append(category.value("name").toString(), category.value("uri").toString().section('/', -1));
        }
    }
    
    emit statusChanged(status());
}
