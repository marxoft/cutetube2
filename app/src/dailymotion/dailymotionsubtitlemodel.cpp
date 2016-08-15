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

#include "dailymotionsubtitlemodel.h"
#include "dailymotion.h"
#include "logger.h"

DailymotionSubtitleModel::DailymotionSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QDailymotion::ResourcesRequest(this))
{
    m_request->setClientId(Dailymotion::clientId());
    m_request->setClientSecret(Dailymotion::clientSecret());
    m_request->setAccessToken(Dailymotion::accessToken());
    m_request->setRefreshToken(Dailymotion::refreshToken());
    
    connect(m_request, SIGNAL(accessTokenChanged(QString)), Dailymotion::instance(), SLOT(setAccessToken(QString)));
    connect(m_request, SIGNAL(refreshTokenChanged(QString)), Dailymotion::instance(), SLOT(setRefreshToken(QString)));
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString DailymotionSubtitleModel::errorString() const {
    return Dailymotion::getErrorString(m_request->result().toMap());
}

QDailymotion::ResourcesRequest::Status DailymotionSubtitleModel::status() const {
    return m_request->status();
}

void DailymotionSubtitleModel::list(const QString &id) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionSubtitleModel::list(). ID: " + id, Logger::MediumVerbosity);
    clear();
    m_id = id;
    m_request->list(QString("/video/%1/subtitles").arg(id), QVariantMap(), Dailymotion::SUBTITLE_FIELDS);
    emit statusChanged(status());
}

void DailymotionSubtitleModel::cancel() {
    m_request->cancel();
}

void DailymotionSubtitleModel::reload() {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    Logger::log("DailymotionSubtitleModel::reload(). ID: " + m_id, Logger::MediumVerbosity);
    clear();
    m_request->list(m_id);
    emit statusChanged(status());
}

void DailymotionSubtitleModel::onRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toMap().value("list").toList()) {
            const QVariantMap subtitle = v.toMap();
            append(subtitle.value("language").toString(), subtitle);
        }
    }
    else {
        Logger::log("DailymotionSubtitleModel::onRequestFinished(). Error: " + errorString());
    }
    
    emit statusChanged(status());
}
