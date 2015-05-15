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

#ifndef FILTERBOX_H
#define FILTERBOX_H

#include <QToolBar>

class QAction;
class QLineEdit;

class FilterBox : public QToolBar
{
    Q_OBJECT
    
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)

public:
    explicit FilterBox(QWidget *parent = 0);
    
    QString text() const;

public Q_SLOTS:
    void setText(const QString &text);
    void clear();

Q_SIGNALS:
    void textChanged(const QString &text);
    
private:
    QLineEdit *m_filterEdit;
    QAction *m_cancelAction;
};

#endif // FILTERBOX_H
