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

#ifndef PLUGINSEARCHDIALOG_H
#define PLUGINSEARCHDIALOG_H

#include "dialog.h"

class PluginSearchTypeModel;
class PluginSearchOrderModel;
class ValueSelector;
class QLineEdit;
class QDialogButtonBox;
class QGridLayout;

class PluginSearchDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit PluginSearchDialog(const QString &service, QWidget *parent = 0);
    
private Q_SLOTS:
    void search();

    void onSearchTextChanged(const QString &text);
    
private:
    PluginSearchTypeModel *m_typeModel;
    PluginSearchOrderModel *m_orderModel;
    
    ValueSelector *m_typeSelector;
    ValueSelector *m_orderSelector;
    QLineEdit *m_searchEdit;
    QDialogButtonBox *m_buttonBox;
    QGridLayout *m_layout;
};
    
#endif // SEARCHDIALOG_H
