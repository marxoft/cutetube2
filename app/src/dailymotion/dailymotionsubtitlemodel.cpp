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

#include "dailymotionsubtitlemodel.h"

DailymotionSubtitleModel::DailymotionSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QDailymotion::ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString DailymotionSubtitleModel::errorString() const {
    return m_request->errorString();
}

QDailymotion::ResourcesRequest::Status DailymotionSubtitleModel::status() const {
    return m_request->status();
}

void DailymotionSubtitleModel::list(const QString &id) {
    if (status() == QDailymotion::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->list(QString("/video/%1/subtitles").arg(id), QVariantMap(), QStringList() << "id" << "language" << "url");
    emit statusChanged(status());
}

void DailymotionSubtitleModel::cancel() {
    m_request->cancel();
}

void DailymotionSubtitleModel::reload() {
    clear();
    m_request->list(m_id);
    emit statusChanged(status());
}

void DailymotionSubtitleModel::onRequestFinished() {
    if (m_request->status() == QDailymotion::ResourcesRequest::Ready) {
        foreach (QVariant v, m_request->result().toList()) {
            QVariantMap subtitle = v.toMap();
            append(subtitle.value("language").toString(), subtitle);
        }
    }
    
    emit statusChanged(status());
}
