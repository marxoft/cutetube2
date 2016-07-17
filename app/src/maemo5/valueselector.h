/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
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

#ifndef VALUESELECTOR_H
#define VALUESELECTOR_H

#include <QMaemo5ValueButton>
#include <QVariant>

class QMaemo5ListPickSelector;
class SelectionModel;

class ValueSelector : public QMaemo5ValueButton
{
    Q_OBJECT
    
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex)
    Q_PROPERTY(QVariant currentValue READ currentValue WRITE setValue NOTIFY valueChanged)

public:
    explicit ValueSelector(const QString &text, QWidget *parent = 0);

    int currentIndex() const;
    void setCurrentIndex(int i);
    
    SelectionModel* model() const;
    virtual void setModel(SelectionModel *model);
    
    virtual QVariant currentValue() const;
    virtual void setValue(const QVariant &v);

protected:
    void showEvent(QShowEvent *e);
    
protected Q_SLOTS:
    virtual void onSelected();

Q_SIGNALS:
    void valueChanged(const QVariant &v);

protected:
    SelectionModel *m_model;
    QMaemo5ListPickSelector *m_selector;
};

#endif // VALUESELECTOR_H
