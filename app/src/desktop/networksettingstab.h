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

#ifndef NETWORKSETTINGSTAB_H
#define NETWORKSETTINGSTAB_H

#include "settingstab.h"

class NetworkProxyTypeModel;
class QCheckBox;
class QComboBox;
class QFormLayout;
class QLineEdit;
class QSpinBox;

class NetworkSettingsTab : public SettingsTab
{
    Q_OBJECT

public:
    explicit NetworkSettingsTab(QWidget *parent = 0);

public Q_SLOTS:
    virtual void restore();
    virtual void save();

private:
    NetworkProxyTypeModel *m_proxyTypeModel;
    
    QCheckBox *m_proxyCheckBox;
    QCheckBox *m_authCheckBox;

    QComboBox *m_proxyTypeSelector;

    QLineEdit *m_hostEdit;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;

    QSpinBox *m_portSpinBox;

    QFormLayout *m_layout;
};

#endif // NETWORKSETTINGSTAB_H
