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

#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <QRegExp>
#include <QStringList>

// Home
static const QString HOME_PATH("/home/user/");

// Plugins
static const QStringList PLUGIN_PATHS = QStringList() << QString("/opt/cutetube2/plugins/")
                                                      << QString(HOME_PATH + "cutetube2/plugins/");

static const QString LIB_PREFIX("lib");
static const QString LIB_SUFFIX(".so");

// Config
static const QString APP_CONFIG_PATH(HOME_PATH + ".config/cuteTube2/");
static const QString PLUGIN_CONFIG_PATH(APP_CONFIG_PATH + "plugins/");

// Downloads
static const QString DOWNLOAD_PATH(HOME_PATH + "MyDocs/cuteTube2/");
static const QRegExp ILLEGAL_FILENAME_CHARS_RE("[\"@&~=\\/:?#!|<>*^]");

// Network
static const int DOWNLOAD_BUFFER_SIZE = 64000;
static const int MAX_CONCURRENT_TRANSFERS = 4;
static const int MAX_REDIRECTS = 8;
static const int MAX_RESULTS = 20;
static const QByteArray USER_AGENT("Wget/1.13.4 (linux-gnu)");

// Version
static const QString VERSION_NUMBER("0.5.2");

#endif // DEFINITIONS_H
