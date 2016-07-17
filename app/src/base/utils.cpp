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

#include "utils.h"
#include <QString>
#include <QRegExp>
#include <QUrl>
#include <QUuid>

Utils::Utils(QObject *parent) :
    QObject(parent)
{
}

QString Utils::createId() {
    const QString uuid = QUuid::createUuid().toString();
    return uuid.mid(1, uuid.size() - 2);
}

QString Utils::formatBytes(qint64 bytes) {
    if (bytes <= 0) {
        return QString("0B");
    }
    
    double kb = 1024;
    double mb = kb * 1024;
    double gb = mb * 1024;

    QString size;

    if (bytes > gb) {
        size = QString::number(bytes / gb, 'f', 2) + "GB";
    }
    else if (bytes > mb) {
        size = QString::number(bytes / mb, 'f', 2) + "MB";
    }
    else if (bytes > kb) {
        size = QString::number(bytes / kb, 'f', 2) + "KB";
    }
    else {
        size = QString::number(bytes) + "B";
    }

    return size;
}

QString Utils::formatLargeNumber(qint64 num) {
    if (num < 1000) {
        return QString::number(num);
    }
    
    double k = 1000;
    double m = k * 1000;

    QString result;

    if (num > m) {
        result = QString::number(num / m, 'f', 1) + "m";
    }
    else {
        result = QString::number(num / k, 'f', 1) + "k";
    }

    return result;
}

QString Utils::formatMSecs(qint64 ms) {    
    return ms > 0 ? formatSecs(ms / 1000) : QString("00:00");
}

QString Utils::formatSecs(qint64 s) {    
    return s > 0 ? QString("%1:%2").arg(s / 60, 2, 10, QChar('0')).arg(s % 60, 2, 10, QChar('0')) : QString("00:00");
}

bool Utils::isLocalFile(const QUrl &url) {
    return (url.scheme() == "file") || (url.toString().startsWith("/"));
}

QString Utils::toRichText(QString s) {
    s.replace("&", "&amp;").replace("<", "&lt;").replace(QRegExp("[\n\r]"), "<br>");
    
    QRegExp re("((http(s|)://|[\\w-_\\.]+@)[^\\s<:\"']+)");
    int pos = 0;

    while ((pos = re.indexIn(s, pos)) != -1) {
        QString link = re.cap(1);
        s.replace(pos, link.size(), QString("<a href='%1'>%2</a>").arg(link.contains('@') ? "mailto:" + link : link).arg(link));
        pos += re.matchedLength() * 2 + 15;
    }

    return s;
}

QString Utils::unescape(const QString &s) {
    int unescapes = 0;
    QByteArray us = s.toUtf8();

    while ((us.contains('%')) && (unescapes < 10)) {
        us = QByteArray::fromPercentEncoding(us);
        unescapes++;
    }

    return QString::fromUtf8(us);
}
