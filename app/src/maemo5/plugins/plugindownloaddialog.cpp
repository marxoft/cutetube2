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

#include "plugindownloaddialog.h"
#include "categorynamemodel.h"
#include "settings.h"
#include "transfers.h"
#include "valueselector.h"
#include <QScrollArea>
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>

PluginDownloadDialog::PluginDownloadDialog(const QString &service, const QString &resourceId, const QString &title,
                                           QWidget *parent) :
    Dialog(parent),
    m_id(resourceId),
    m_title(title),
    m_streamModel(new PluginStreamModel(this)),
    m_categoryModel(new CategoryNameModel(this)),
    m_scrollArea(new QScrollArea(this)),
    m_audioCheckBox(new QCheckBox(tr("Convert to audio file"), this)),
    m_streamSelector(new ValueSelector(tr("Video format"), this)),
    m_categorySelector(new ValueSelector(tr("Category"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Download video"));
    
    m_streamModel->setService(service);
    
    m_streamSelector->setModel(m_streamModel);
    m_categorySelector->setModel(m_categoryModel);
    m_categorySelector->setValue(Settings::instance()->defaultCategory());
    m_categorySelector->setEnabled(m_categoryModel->rowCount() > 0);
    m_audioCheckBox->setEnabled(QFile::exists("/usr/bin/ffmpeg") || QFile::exists("/usr/bin/avconv"));
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(m_streamSelector, 0, 0);
    vbox->addWidget(m_audioCheckBox, 1, 0);
    vbox->addWidget(m_categorySelector, 2, 0);
    vbox->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_buttonBox, Qt::AlignBottom);
    m_layout->setStretch(0, 1);
    
    connect(m_streamModel, SIGNAL(statusChanged(ResourcesRequest::Status)), this,
            SLOT(onStreamModelStatusChanged(ResourcesRequest::Status)));
    connect(m_categorySelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onCategoryChanged()));
    connect(m_streamSelector, SIGNAL(valueChanged(QVariant)), this, SLOT(onStreamChanged()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(addDownload()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));    
}

void PluginDownloadDialog::showEvent(QShowEvent *e) {
    Dialog::showEvent(e);
    m_streamModel->list(m_id);
}

void PluginDownloadDialog::onCategoryChanged() {
    Settings::instance()->setDefaultCategory(m_categorySelector->valueText());
}

void PluginDownloadDialog::onStreamChanged() {
    Settings::instance()->setDefaultDownloadFormat(m_streamModel->service(), m_streamSelector->valueText());
}

void PluginDownloadDialog::onStreamModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case ResourcesRequest::Ready:
        if (m_streamModel->rowCount() > 0) {
            m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                                   Settings::instance()->defaultDownloadFormat(m_streamModel->service()))));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("No streams available for '%1'").arg(m_title));
        }
        
        break;
    case ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), tr("No streams available for '%1'").arg(m_title));
        break;
    default:
        break;
    }
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_streamModel->rowCount() > 0);
    hideProgressIndicator();
}

void PluginDownloadDialog::addDownload() {
    QString streamId = m_streamSelector->currentValue().toMap().value("id").toString();
    QString category = m_categorySelector->valueText();
    Transfers::instance()->addDownloadTransfer(m_streamModel->service(), m_id, streamId, m_title, category, QString(),
                                               m_audioCheckBox->isChecked());
    
    accept();
}
