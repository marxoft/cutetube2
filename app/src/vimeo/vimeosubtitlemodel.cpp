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

#include "vimeosubtitlemodel.h"

VimeoSubtitleModel::VimeoSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QVimeo::ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString VimeoSubtitleModel::errorString() const {
    return m_request->errorString();
}

QVimeo::ResourcesRequest::Status VimeoSubtitleModel::status() const {
    return m_request->status();
}

void VimeoSubtitleModel::list(const QString &id) {
    if (status() == QVimeo::ResourcesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->list(QString("/videos/%1/texttracks").arg(id));
    emit statusChanged(status());
}

void VimeoSubtitleModel::cancel() {
    m_request->cancel();
}

void VimeoSubtitleModel::reload() {
    clear();
    m_request->list(m_id);
    emit statusChanged(status());
}

void VimeoSubtitleModel::onRequestFinished() {
    if (m_request->status() == QVimeo::ResourcesRequest::Ready) {
        foreach (const QVariant &v, m_request->result().toList()) {
            const QVariantMap subtitle = v.toMap();
            append(subtitle.value("language").toString(), subtitle);
        }
    }
    
    emit statusChanged(status());
}
