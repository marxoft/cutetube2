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
#include "definitions.h"
#include "logger.h"
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QUrl>

VideoLauncher::VideoLauncher(QObject *parent) :
    QObject(parent)
{
}

bool VideoLauncher::playVideo(const QString &url) {
    if (!QDir().mkpath(APP_CONFIG_PATH)) {
        Logger::log(QString("VideoLauncher::playVideo(). URL: %1. Cannot create directory: %2")
                           .arg(url).arg(APP_CONFIG_PATH));
        return false;
    }
    
    QFile file(APP_CONFIG_PATH + "video.ram");
    
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        Logger::log(QString("VideoLauncher::playVideo(). URL: %1, Cannot write to file: %2. Error: %3")
                           .arg(url).arg(file.fileName()).arg(file.errorString()));
        return false;
    }
    
    QTextStream stream(&file);
    stream << url;
    file.close();
    
    if (!QDesktopServices::openUrl(QUrl::fromLocalFile(file.fileName()))) {
        Logger::log(QString("VideoLauncher::playVideo(). URL: %1. No handler for file: %2")
                           .arg(url).arg(file.fileName()));
        return false;
    }
    
    Logger::log("VideoLauncher::playVideo(). URL " + url, Logger::LowVerbosity);
    return true;
}
