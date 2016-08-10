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

#include "transferdelegate.h"
#include "transfermodel.h"
#include <QApplication>
#include <QStyleOptionProgressBar>

TransferDelegate::TransferDelegate(QObject *parent) :
    QStyledItemDelegate(parent)
{
}

void TransferDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    if (index.column() == 3) {
        QStyleOptionProgressBar bar;
        bar.rect = option.rect;
        bar.minimum = 0;
        bar.maximum = 100;
        bar.progress = index.data(TransferModel::ProgressRole).toInt();
        bar.textVisible = true;
        bar.text = index.data(TransferModel::ProgressStringRole).toString();
        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &bar, painter);
    }
    else {
        QStyledItemDelegate::paint(painter, option, index);
    }
}
