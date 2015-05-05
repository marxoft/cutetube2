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

#include "videoplayer.h"
#include "settings.h"
#include <qplatformdefs.h>
#include <QUrl>
#ifndef SYMBIAN_OS
#include <QProcess>
#endif
#ifdef Q_WS_MAEMO_5
#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusInterface>
#elif defined (MEEGO_EDITION_HARMATTAN)
#include <contentaction/contentaction.h>
#elif defined (SYMBIAN_OS)
#include <QTemporaryFile>
#include <QDesktopServices>
#include <QTimer>
#include <QDir>
#endif
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

VideoPlayer::VideoPlayer(QObject *parent) :
    QObject(parent)
{
}

void VideoPlayer::playVideo(const QString &url) {
#ifdef CUTETUBE_DEBUG
    qDebug() << "VideoPlayer::playVideo" << url;
#endif
#ifdef SYMBIAN_OS
    QTemporaryFile *temp = new QTemporaryFile;
    temp->setFileName(QDir::tempPath() + "/cutetube2.ram");

    if ((temp->open()) && (temp->write(QByteArray::fromPercentEncoding(url.toUtf8())) > 0)) {
        QDesktopServices::openUrl(QUrl::fromLocalFile(temp->fileName()));
    }

    temp->close();
    QTimer::singleShot(5000, temp, SLOT(deleteLater()));
#else
#ifdef Q_OS_LINUX
    if (Settings::instance()->videoPlayer() == "other") {
        QStringList args = Settings::instance()->videoPlayerCommand().split(' ', QString::SkipEmptyParts);
        QProcess *player = new QProcess;
        QObject::connect(player, SIGNAL(finished(int, QProcess::ExitStatus)), player, SLOT(deleteLater()));
        player->start(args.takeFirst(), args << QByteArray::fromPercentEncoding(url.toUtf8()));
    }
    else if (Settings::instance()->videoPlayer() == "mplayer") {
        QStringList args;
        args << "-cache" << "2048" << "-fs" << QByteArray::fromPercentEncoding(url.toUtf8());
        QProcess *player = new QProcess;
        QObject::connect(player, SIGNAL(finished(int, QProcess::ExitStatus)), player, SLOT(deleteLater()));
        player->start("/usr/bin/mplayer", args);
    }
#ifdef MEEGO_EDITION_HARMATTAN
    else if (Settings::instance()->videoPlayer() == "mediaplayer") {
        ContentAction::Action::launcherAction("videosuiteinterface.desktop", QStringList(url)).trigger();
    }
    else if (Settings::instance()->videoPlayer() == "kmplayer") {
        ContentAction::Action::launcherAction("kmplayer.desktop", QStringList(url)).trigger();
    }
#endif
#ifdef Q_WS_MAEMO_5
    else {
        QDBusConnection bus = QDBusConnection::sessionBus();
        QDBusInterface dbus_iface("com.nokia." + Settings::instance()->videoPlayer(), "/com/nokia/" + Settings::instance()->videoPlayer(), "com.nokia." + Settings::instance()->videoPlayer(), bus);
        dbus_iface.call("mime_open", url);
    }
#endif
#endif
#endif
}
