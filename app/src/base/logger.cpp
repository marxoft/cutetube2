/*
 * Copyright (C) 2016 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "logger.h"
#include <QDateTime>
#include <iostream>

int Logger::vb = 0;

Logger::Logger(QObject *parent) :
    QObject(parent)
{
}

int Logger::verbosity() {
    return vb;
}

void Logger::setVerbosity(int v) {
    vb = v;
}

void Logger::log(const QString &message, int minimumVerbosity) {
    if (minimumVerbosity <= vb) {
        std::cout << QString("%1: %2\n").arg(QDateTime::currentDateTime().toString(Qt::ISODate)).arg(message)
                                        .toUtf8().constData();
    }
}
