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
#include <QFile>
#include <QTextStream>
#include <iostream>

QString Logger::fn;
int Logger::vb = 0;

Logger::Logger(QObject *parent) :
    QObject(parent)
{
}

QString Logger::fileName() {
    return fn;
}

void Logger::setFileName(const QString &f) {
    fn = f;
}

QString Logger::text() {
    QString output;
    
    if (!fn.isEmpty()) {
        QFile file(fn);
        
        if (file.open(QFile::ReadOnly | QFile::Text)) {
            QTextStream stream(&file);

            while (!stream.atEnd()) {
                output.append(stream.readLine());
                output.append("\n");
            }

            file.close();
        }
    }
    
    return output;
}

int Logger::verbosity() {
    return vb;
}

void Logger::setVerbosity(int v) {
    vb = v;
}

void Logger::clear() {
    if (!fn.isEmpty()) {
        QFile::remove(fn);
    }
}

void Logger::log(const QString &message, int minimumVerbosity) {
    if (minimumVerbosity <= vb) {
        const QString date = QDateTime::currentDateTime().toString(Qt::ISODate);
        QString output = QString("%1: %2\n").arg(date).arg(message);
        
        if (!fn.isEmpty()) {
            QFile file(fn);
            
            if (file.open(QFile::Append | QFile::Text)) {
                QTextStream stream(&file);
                stream << output;
                file.close();
                return;
            }
            
            output = tr("%1: Cannot write to log file '%2'. Error: %3").arg(date).arg(fn).arg(file.errorString());
        }
        
        std::cout << output.toUtf8().constData();
    }
}
