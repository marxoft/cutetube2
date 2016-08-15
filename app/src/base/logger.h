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

#ifndef LOGGER_H
#define LOGGER_H

#include <QObject>

class Logger : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString fileName READ fileName WRITE setFileName)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(int verbosity READ verbosity WRITE setVerbosity)
    
    Q_ENUMS(Verbosity)

public:
    enum Verbosity {
        NoVerbosity = 0,
        LowestVerbosity,
        LowVerbosity,
        MediumVerbosity,
        HighVerbosity,
        HighestVerbosity
    };
    
    explicit Logger(QObject *parent = 0);

    static QString fileName();
    
    static QString text();
    
    static int verbosity();

public Q_SLOTS:
    static void setFileName(const QString &f);

    static void setVerbosity(int v);

    static void clear();

    static void log(const QString &message, int minimumVerbosity = LowestVerbosity);

private:
    static QString fn;
    static int vb;
};

#endif // LOGGER_H
