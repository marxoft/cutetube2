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

#ifndef ACTIVECOLORMODEL_H
#define ACTIVECOLORMODEL_H

#include "selectionmodel.h"

class ActiveColorModel : public SelectionModel
{
    Q_OBJECT
    
public:
    explicit ActiveColorModel(QObject *parent = 0) :
        SelectionModel(parent)
    {
        append("#66b907", "#66b907");
        append("#418b11", "#418b11");
        append("#37790c", "#37790c");
        append("#346905", "#346905");
        append("#0fa9cd", "#0fa9cd");
        append("#0881cb", "#0881cb");
        append("#066bbe", "#066bbe");
        append("#2054b1", "#2054b1");
        append("#6705bd", "#6705bd");
        append("#8a12bc", "#8a12bc");
        append("#cd0fbc", "#cd0fbc");
        append("#e805a3", "#e805a3");
        append("#ef5906", "#ef5906");
        append("#ea6910", "#ea6910");
        append("#f7751e", "#f7751e");
        append("#ff8806", "#ff8806");
        append("#ed970c", "#ed970c");
        append("#f2b317", "#f2b317");
    }
};

#endif // ACTIVECOLORMODEL_H
