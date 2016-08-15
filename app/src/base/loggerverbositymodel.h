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

#ifndef LOGGERVERBOSITYMODEL_H
#define LOGGERVERBOSITYMODEL_H

#include "selectionmodel.h"
#include "logger.h"

class LoggerVerbosityModel : public SelectionModel
{
    Q_OBJECT
    
public:
    explicit LoggerVerbosityModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append(tr("No logging"), Logger::NoVerbosity);
        append(tr("Lowest"), Logger::LowestVerbosity);
        append(tr("Low"), Logger::LowVerbosity);
        append(tr("Medium"), Logger::MediumVerbosity);
        append(tr("High"), Logger::HighVerbosity);
        append(tr("Highest"), Logger::HighestVerbosity);
    }
};

#endif // LOGGERVERBOSITYMODEL_H
