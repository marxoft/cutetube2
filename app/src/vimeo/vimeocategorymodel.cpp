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

#include "vimeocategorymodel.h"
#include "logger.h"
#include "vimeo.h"

VimeoCategoryModel::VimeoCategoryModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QVimeo::ResourcesRequest(this))
{
    m_request->setClientId(Vimeo::clientId());
    m_request->setClientSecret(Vimeo::clientSecret());
    m_request->setAccessToken(Vimeo::accessToken());
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Vimeo::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString VimeoCategoryModel::errorString() const {
    return Vimeo::getErrorString(m_request->result().toMap());
}

QVimeo::ResourcesRequest::Status VimeoCategoryModel::status() const {
    return m_request->status();
}

void VimeoCategoryModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("VimeoCategoryModel::list(). Resource path: " + resourcePath, Logger::HighVerbosity);
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
    
    Logger::log("VimeoCategoryModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->list(m_resourcePath, m_filters);
    emit statusChanged(status());
}

void VimeoCategoryModel::onRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toMap().value("data").toList()) {
            const QVariantMap category = v.toMap();
            append(category.value("name").toString(), category.value("uri").toString().section('/', -1));
        }
    }
    else {
        Logger::log("VimeoCategoryModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
