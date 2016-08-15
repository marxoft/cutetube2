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

#include "dailymotioncategorymodel.h"
#include "dailymotion.h"
#include "logger.h"

DailymotionCategoryModel::DailymotionCategoryModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QDailymotion::ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString DailymotionCategoryModel::errorString() const {
    return Dailymotion::getErrorString(m_request->result().toMap());
}

QDailymotion::ResourcesRequest::Status DailymotionCategoryModel::status() const {
    return m_request->status();
}

void DailymotionCategoryModel::list(const QString &resourcePath, const QVariantMap &filters) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionCategoryModel::list(). Resource path: " + resourcePath, Logger::HighVerbosity);
    clear();
    m_resourcePath = resourcePath;
    m_filters = filters;
    m_request->list(resourcePath, filters, Dailymotion::CATEGORY_FIELDS);
    emit statusChanged(status());
}

void DailymotionCategoryModel::cancel() {
    m_request->cancel();
}

void DailymotionCategoryModel::reload() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionCategoryModel::reload(). Resource path: " + m_resourcePath, Logger::HighVerbosity);
    clear();
    m_request->list(m_resourcePath, m_filters, Dailymotion::CATEGORY_FIELDS);
    emit statusChanged(status());
}

void DailymotionCategoryModel::onRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toMap().value("list").toList()) {
            const QVariantMap category = v.toMap();
            append(category.value("name").toString(), category.value("id").toString());
        }
    }
    else {
        Logger::log("DailymotionCategoryModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
