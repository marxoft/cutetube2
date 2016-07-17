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

#include "dialog.h"

Dialog::Dialog(QWidget *parent) :
    QDialog(parent),
    m_progressCount(0)
{
}

bool Dialog::isBusy() const {
    return testAttribute(Qt::WA_Maemo5ShowProgressIndicator);
}

void Dialog::showProgressIndicator() {
    m_progressCount++;

    if (isVisible()) {
        setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
    }
}

void Dialog::hideProgressIndicator() {
    if (m_progressCount > 0) {
        m_progressCount--;
        
        if (m_progressCount == 0) {
            setAttribute(Qt::WA_Maemo5ShowProgressIndicator, false);
        }
    }
}

void Dialog::showEvent(QShowEvent *e) {
    QDialog::showEvent(e);

    if (m_progressCount > 0) {
        setAttribute(Qt::WA_Maemo5ShowProgressIndicator, true);
    }
}
