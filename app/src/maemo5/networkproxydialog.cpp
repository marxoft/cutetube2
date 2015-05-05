/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "networkproxydialog.h"
#include "networkproxytypemodel.h"
#include "settings.h"
#include "valueselector.h"
#include <QGroupBox>
#include <QLineEdit>
#include <QGridLayout>
#include <QScrollArea>
#include <QLabel>
#include <QDialogButtonBox>
#include <QIntValidator>

NetworkProxyDialog::NetworkProxyDialog(QWidget *parent) :
    Dialog(parent),
    m_proxyTypeSelector(new ValueSelector(tr("Proxy type"), this)),
    m_hostEdit(new QLineEdit(this)),
    m_portEdit(new QLineEdit(this)),
    m_userEdit(new QLineEdit(this)),
    m_passEdit(new QLineEdit(this))
{
    setWindowTitle(tr("Network proxy"));
    
    m_proxyTypeSelector->setModel(new NetworkProxyTypeModel(this));
    m_proxyTypeSelector->setValue(Settings::instance()->networkProxyType());

    m_hostEdit->setMinimumWidth(380);
    m_hostEdit->setText(Settings::instance()->networkProxyHost());
    m_portEdit->setValidator(new QIntValidator(0, 100000, this));
    m_portEdit->setText(QString::number(Settings::instance()->networkProxyPort()));
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setText(Settings::instance()->networkProxyPassword());
    m_userEdit->setText(Settings::instance()->networkProxyUsername());

    QGroupBox *proxyGroup = new QGroupBox(tr("Use network proxy"), this);
    proxyGroup->setCheckable(true);
    proxyGroup->setChecked(Settings::instance()->networkProxyEnabled());
    
    QGridLayout *proxyGrid = new QGridLayout(proxyGroup);
    proxyGrid->setContentsMargins(0, 0, 0, 0);
    proxyGrid->addWidget(m_proxyTypeSelector, 0, 0, 1, 2);
    proxyGrid->addWidget(new QLabel(tr("Host"), this), 1, 0, 1, 1);
    proxyGrid->addWidget(new QLabel(tr("Port"), this), 1, 1, 1, 1);
    proxyGrid->addWidget(m_hostEdit, 2, 0, 1, 1);
    proxyGrid->addWidget(m_portEdit, 2, 1, 1, 1);
    proxyGrid->addWidget(new QLabel(tr("Username"), this), 3, 0, 1, 2);
    proxyGrid->addWidget(m_userEdit, 4, 0, 1, 2);
    proxyGrid->addWidget(new QLabel(tr("Password"), this), 5, 0, 1, 2);
    proxyGrid->addWidget(m_passEdit, 6, 0, 1, 2);

    QWidget *scrollWidget = new QWidget(this);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->setContentsMargins(0, 0, 0, 0);
    vbox->addWidget(proxyGroup);
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    hbox->addWidget(scrollArea);
    hbox->addWidget(buttonBox);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
    connect(proxyGroup, SIGNAL(toggled(bool)), Settings::instance(), SLOT(setNetworkProxyEnabled(bool)));
    connect(m_proxyTypeSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(setNetworkProxyType(QVariant)));
    connect(m_hostEdit, SIGNAL(textEdited(QString)), Settings::instance(), SLOT(setNetworkProxyHost(QString)));
    connect(m_portEdit, SIGNAL(textEdited(QString)), this, SLOT(setNetworkProxyPort(QString)));
    connect(m_userEdit, SIGNAL(textEdited(QString)), Settings::instance(), SLOT(setNetworkProxyUsername(QString)));
    connect(m_passEdit, SIGNAL(textEdited(QString)), Settings::instance(), SLOT(setNetworkProxyPassword(QString)));
}

void NetworkProxyDialog::accept() {
    Settings::instance()->setNetworkProxy();
    Dialog::accept();
}

void NetworkProxyDialog::setNetworkProxyPort(const QString &port) {
    Settings::instance()->setNetworkProxyPort(port.toInt());
}

void NetworkProxyDialog::setNetworkProxyType(const QVariant &type) {
    Settings::instance()->setNetworkProxyType(type.toInt());
}
