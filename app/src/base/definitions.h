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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QRegExp>
#if QT_VERSION >= 0x050000
#include <QStandardPaths>
#else
#include <qplatformdefs.h>
#include <QDesktopServices>
#endif

static const int MAX_CONCURRENT_TRANSFERS = 4;
static const int MAX_REDIRECTS = 8;

static const int MAX_RESULTS = 20;

static const QRegExp ILLEGAL_FILENAME_CHARS_RE("[\"@&~=\\/:?#!|<>*^]");

static const QString VERSION_NUMBER("0.0.2");

static const QString PLUGIN_PATH("/opt/cutetube2/plugins/");
#if QT_VERSION >= 0x050000
static const QString DATABASE_PATH(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/cuteTube2/");
static const QString DOWNLOAD_PATH(QStandardPaths::writableLocation(QStandardPaths::DownloadLocation) + "/cuteTube2/");
static const QString STORAGE_PATH(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + "/cuteTube2/");
#else
static const QString DATABASE_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.config/cuteTube2/");
#if (defined Q_WS_MAEMO_5) || (defined MEEGO_EDITION_HARMATTAN)
static const QString DOWNLOAD_PATH("/home/user/MyDocs/cuteTube2/");
#else
static const QString DOWNLOAD_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/cuteTube2/");
#endif
static const QString STORAGE_PATH(QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/.config/cuteTube2/");
#endif

#endif // DEFINITIONS_H
