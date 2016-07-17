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

#include "valueselector.h"
#include "selectionmodel.h"
#include <QMaemo5ListPickSelector>

ValueSelector::ValueSelector(const QString &text, QWidget *parent) :
    QMaemo5ValueButton(text, parent),
    m_model(0),
    m_selector(new QMaemo5ListPickSelector(this))
{
    setPickSelector(m_selector);
    setValueText(tr("None chosen"));
    connect(m_selector, SIGNAL(selected(QString)), this, SLOT(onSelected()));
}

int ValueSelector::currentIndex() const {
    return m_selector->currentIndex();
}

void ValueSelector::setCurrentIndex(int i) {
    m_selector->setCurrentIndex(i);
    setValueText(m_model->data(m_model->index(i, 0), SelectionModel::NameRole).toString());
}

SelectionModel* ValueSelector::model() const {
    return m_model;
}

void ValueSelector::setModel(SelectionModel *model) {
    m_model = model;
    m_selector->setModel(model);
    m_selector->setModelColumn(0);
}

void ValueSelector::setValue(const QVariant &v) {
    if (!m_model) {
        return;
    }

    bool found = false;
    int i = 0;

    while ((!found) && (i < m_model->rowCount())) {
        found = m_model->data(m_model->index(i, 0), SelectionModel::ValueRole) == v;

        if (found) {
            m_selector->setCurrentIndex(i);
        }

        i++;
    }

    if (!found) {
        setValueText(tr("None chosen"));
    }
}

QVariant ValueSelector::currentValue() const {
    return !m_model ? QVariant() : m_model->data(m_model->index(m_selector->currentIndex(), 0),
                                                 SelectionModel::ValueRole);
}

void ValueSelector::showEvent(QShowEvent *e) {
    QMaemo5ValueButton::showEvent(e);
    
    if (valueText().isEmpty()) {
        m_selector->setCurrentIndex(0);
    }    
}

void ValueSelector::onSelected() {
    if (!currentValue().isNull()) {
        emit valueChanged(currentValue());
    }
}
