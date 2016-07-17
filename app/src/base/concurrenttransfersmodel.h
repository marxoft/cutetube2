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

#ifndef CONCURRENTTRANSERSMODEL_H
#define CONCURRENTTRANSERSMODEL_H

#include "selectionmodel.h"
#include "definitions.h"

class ConcurrentTransfersModel : public SelectionModel
{
    Q_OBJECT
    
public:
    explicit ConcurrentTransfersModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        for (int i = 1; i <= MAX_CONCURRENT_TRANSFERS; i++) {
            append(QString::number(i), i);
        }
    }
};

#endif // CONCURRENTTRANSERSMODEL_H
