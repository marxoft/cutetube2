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

#include "videolauncher.h"
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QTimer>
#include <QDir>
#include <QUrl>

VideoLauncher::VideoLauncher(QObject *parent) :
    QObject(parent)
{
}

bool VideoLauncher::playVideo(const QString &url) {
    bool ok = false;
    QTemporaryFile *temp = new QTemporaryFile;
    temp->setFileName(QDir::tempPath() + "/cutetube2.ram");
    
    if ((temp->open()) && (temp->write(QByteArray::fromPercentEncoding(url.toUtf8())) > 0)) {
        ok = QDesktopServices::openUrl(QUrl::fromLocalFile(temp->fileName()));
    }
    
    temp->close();
    QTimer::singleShot(10000, temp, SLOT(deleteLater()));
    return ok;
}
