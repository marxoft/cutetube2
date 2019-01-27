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
#include "settings.h"
#include <QProcess>
#include <contentaction/contentaction.h>

VideoLauncher::VideoLauncher(QObject *parent) :
    QObject(parent)
{
}

bool VideoLauncher::playVideo(const QString &url) {
    const QString &player = Settings::videoPlayer();

    if (player == "other") {
        return QProcess::startDetached(Settings::videoPlayerCommand().replace("%u", url));
    }
    else {
        ContentAction::Action::launcherAction(player + ".desktop", QStringList(url)).trigger();
        return true;
    }
}
