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

#ifndef VALUESELECTORACTION_H
#define VALUESELECTORACTION_H

#include <QWidgetAction>

class SelectionModel;

class ValueSelectorAction : public QWidgetAction
{
    Q_OBJECT
    
    Q_PROPERTY(QString valueText READ valueText NOTIFY valueChanged)
    Q_PROPERTY(QVariant currentValue READ currentValue WRITE setValue NOTIFY valueChanged)

public:
    explicit ValueSelectorAction(QWidget *parent = 0);

    SelectionModel* model() const;
    void setModel(SelectionModel *model);
    
    QString valueText() const;

    QVariant currentValue() const;

    QWidget* createWidget(QWidget *parent);

public Q_SLOTS:
    void setValue(const QVariant &v);
    
protected Q_SLOTS:
    void onValueChanged(const QVariant &v);

Q_SIGNALS:
    void valueChanged(const QVariant &v);

protected:
    SelectionModel *m_model;
    QString m_text;
    QVariant m_value;
};

#endif // VALUESELECTORACTION_H
