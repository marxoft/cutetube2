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

#include "networkproxydialog.h"
#include "networkproxytypemodel.h"
#include "settings.h"
#include "valueselector.h"
#include <QGroupBox>
#include <QLineEdit>
#include <QVBoxLayout>
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
    setMinimumHeight(360);
    
    m_proxyTypeSelector->setModel(new NetworkProxyTypeModel(this));
    m_proxyTypeSelector->setValue(Settings::networkProxyType());

    m_hostEdit->setMinimumWidth(380);
    m_hostEdit->setText(Settings::networkProxyHost());
    m_portEdit->setValidator(new QIntValidator(0, 100000, this));
    m_portEdit->setText(QString::number(Settings::networkProxyPort()));
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setText(Settings::networkProxyPassword());
    m_userEdit->setText(Settings::networkProxyUsername());

    QGroupBox *proxyGroup = new QGroupBox(tr("Use network proxy"), this);
    proxyGroup->setCheckable(true);
    proxyGroup->setChecked(Settings::networkProxyEnabled());
    
    QVBoxLayout *proxyLayout = new QVBoxLayout(proxyGroup);
    proxyLayout->setContentsMargins(0, 0, 0, 0);
    proxyLayout->addWidget(m_proxyTypeSelector);
    proxyLayout->addWidget(new QLabel(tr("Host"), this));
    proxyLayout->addWidget(m_hostEdit);
    proxyLayout->addWidget(new QLabel(tr("Port"), this));
    proxyLayout->addWidget(m_portEdit);
    proxyLayout->addWidget(new QLabel(tr("Username"), this));
    proxyLayout->addWidget(m_userEdit);
    proxyLayout->addWidget(new QLabel(tr("Password"), this));
    proxyLayout->addWidget(m_passEdit);

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
    Settings::setNetworkProxy();
    Dialog::accept();
}

void NetworkProxyDialog::setNetworkProxyPort(const QString &port) {
    Settings::setNetworkProxyPort(port.toInt());
}

void NetworkProxyDialog::setNetworkProxyType(const QVariant &type) {
    Settings::setNetworkProxyType(type.toInt());
}
