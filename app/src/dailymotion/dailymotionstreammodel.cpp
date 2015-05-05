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

#include "dailymotionstreammodel.h"

DailymotionStreamModel::DailymotionStreamModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QDailymotion::StreamsRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString DailymotionStreamModel::errorString() const {
    return m_request->errorString();
}

QDailymotion::StreamsRequest::Status DailymotionStreamModel::status() const {
    return m_request->status();
}

void DailymotionStreamModel::list(const QString &id) {
    if (status() == QDailymotion::StreamsRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->list(id);
    emit statusChanged(status());
}

void DailymotionStreamModel::cancel() {
    m_request->cancel();
}

void DailymotionStreamModel::reload() {
    clear();
    m_request->list(m_id);
    emit statusChanged(status());
}

void DailymotionStreamModel::onRequestFinished() {
    if (m_request->status() == QDailymotion::StreamsRequest::Ready) {
        foreach (QVariant v, m_request->result().toList()) {
            QVariantMap stream = v.toMap();
            append(QString("%1p %2").arg(stream.value("height").toInt()).arg(stream.value("description").toString()),
                   stream);
        }
    }
    
    emit statusChanged(status());
}
