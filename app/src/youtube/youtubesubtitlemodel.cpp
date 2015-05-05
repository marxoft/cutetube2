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

#include "youtubesubtitlemodel.h"

YouTubeSubtitleModel::YouTubeSubtitleModel(QObject *parent) :
    SelectionModel(parent),
    m_request(new QYouTube::SubtitlesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

QString YouTubeSubtitleModel::errorString() const {
    return m_request->errorString();
}

QYouTube::SubtitlesRequest::Status YouTubeSubtitleModel::status() const {
    return m_request->status();
}

void YouTubeSubtitleModel::list(const QString &id) {
    if (status() == QYouTube::SubtitlesRequest::Loading) {
        return;
    }
    
    clear();
    m_id = id;
    m_request->list(id);
    emit statusChanged(status());
}

void YouTubeSubtitleModel::cancel() {
    m_request->cancel();
}

void YouTubeSubtitleModel::reload() {
    clear();
    m_request->list(m_id);
    emit statusChanged(status());
}

void YouTubeSubtitleModel::onRequestFinished() {
    if (m_request->status() == QYouTube::SubtitlesRequest::Ready) {
        foreach (QVariant v, m_request->result().toList()) {
            QVariantMap subtitle = v.toMap();
            append(subtitle.value("translatedLanguage").toString(), subtitle);
        }
    }
    
    emit statusChanged(status());
}
