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
#include "logger.h"
#include "settings.h"
#include <QProcess>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>

VideoLauncher::VideoLauncher(QObject *parent) :
    QObject(parent)
{
}

bool VideoLauncher::playVideo(const QString &url) {
    const QString player = Settings::videoPlayer();
    Logger::log(QString("VideoLauncher::playVideo(). URL: %1, Player: %2").arg(url).arg(player), Logger::LowVerbosity);
    
    if (player == "other") {
        const QString command = Settings::videoPlayerCommand().replace("%u", url);
        
        if (!QProcess::startDetached(command)) {
            Logger::log(QString("VideoLauncher::playVideo(). Error playing URL: %1 with command: %2")
                               .arg(url).arg(command));
            return false;
        }
    }
    else if (player == "mplayer") {
        if (!QProcess::startDetached("/usr/bin/mplayer", QStringList() << "-cache" << "2048" << "-fs" << url)) {
            Logger::log(QString("VideoLauncher::playVideo(). Error playing URL: %1 with mplayer").arg(url));
            return false;
        }
    }
    else {
        QDBusConnection bus = QDBusConnection::sessionBus();
        QDBusInterface dbus_iface("com.nokia." + player, "/com/nokia/" + player, "com.nokia." + player, bus);
        dbus_iface.call("mime_open", url);
    }
    
    return true;
}
