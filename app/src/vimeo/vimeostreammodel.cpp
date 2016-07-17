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

#include "vimeostreammodel.h"

VimeoStreamModel::VimeoStreamModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QVimeo::StreamsRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString VimeoStreamModel::errorString() const {
    return m_request->errorString();
}

QVimeo::StreamsRequest::Status VimeoStreamModel::status() const {
    return m_request->status();
}

void VimeoStreamModel::list(const QString &id) {
    if (status() == QVimeo::StreamsRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->list(id);
    emit statusChanged(status());
}

void VimeoStreamModel::cancel() {
    m_request->cancel();
}

void VimeoStreamModel::reload() {
    clear();
    m_request->list(m_id);
    emit statusChanged(status());
}

void VimeoStreamModel::onRequestFinished() {
    if (m_request->status() == QVimeo::StreamsRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toList()) {
            const QVariantMap stream = v.toMap();
            append(QString("%1p %2").arg(stream.value("height").toInt()).arg(stream.value("description").toString()),
                   stream);
        }
    }
    
    emit statusChanged(status());
}
