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

#include "dailymotiondownloaddialog.h"
#include "categorynamemodel.h"
#include "resources.h"
#include "settings.h"
#include "valueselector.h"
#include <QScrollArea>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QMessageBox>

DailymotionDownloadDialog::DailymotionDownloadDialog(QWidget *parent) :
    Dialog(parent),
    m_streamModel(new DailymotionStreamModel(this)),
    m_subtitleModel(new DailymotionSubtitleModel(this)),
    m_categoryModel(new CategoryNameModel(this)),
    m_scrollArea(new QScrollArea(this)),
    m_subtitleCheckBox(new QCheckBox(tr("Download subtitles"), this)),
    m_commandCheckBox(new QCheckBox(tr("Override global custom command"), this)),
    m_commandEdit(new QLineEdit(this)),
    m_streamSelector(new ValueSelector(tr("Video format"), this)),
    m_subtitleSelector(new ValueSelector(tr("Subtitles language"), this)),
    m_categorySelector(new ValueSelector(tr("Category"), this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QHBoxLayout(this))
{
    setWindowTitle(tr("Download video"));
    setMinimumHeight(360);
    
    m_streamSelector->setModel(m_streamModel);
    m_subtitleSelector->setModel(m_subtitleModel);
    m_subtitleSelector->setEnabled(false);
    m_subtitleSelector->setCurrentIndex(qMax(0, m_subtitleModel->match("name",
                                             Settings::subtitlesLanguage())));
    m_categorySelector->setModel(m_categoryModel);
    m_categorySelector->setValue(Settings::defaultCategory());
    m_categorySelector->setEnabled(m_categoryModel->rowCount() > 0);
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->addWidget(m_streamSelector);
    vbox->addWidget(m_subtitleCheckBox);
    vbox->addWidget(m_subtitleSelector);
    vbox->addWidget(m_categorySelector);
    vbox->addWidget(new QLabel(tr("Custom command (%f for filename)"), this));
    vbox->addWidget(m_commandEdit);
    vbox->addWidget(m_commandCheckBox);
    vbox->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_buttonBox, Qt::AlignBottom);
    m_layout->setStretch(0, 1);
    
    connect(m_streamModel, SIGNAL(statusChanged(QDailymotion::StreamsRequest::Status)), this,
            SLOT(onStreamModelStatusChanged(QDailymotion::StreamsRequest::Status)));
    connect(m_subtitleModel, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)), this,
            SLOT(onSubtitleModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_subtitleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onSubtitleCheckBoxToggled(bool)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));    
}

QString DailymotionDownloadDialog::videoId() const {
    return m_videoId;
}

QString DailymotionDownloadDialog::streamId() const {
    return m_streamSelector->currentValue().toMap().value("id").toString();
}

QString DailymotionDownloadDialog::subtitlesLanguage() const {
    return m_subtitleCheckBox->isChecked()
           ? m_subtitleSelector->currentValue().toMap().value("id").toString()
           : QString();
}

QString DailymotionDownloadDialog::category() const {
    return m_categorySelector->valueText();
}

QString DailymotionDownloadDialog::customCommand() const {
    return m_commandEdit->text();
}

bool DailymotionDownloadDialog::customCommandOverrideEnabled() const {
    return m_commandCheckBox->isChecked();
}

void DailymotionDownloadDialog::accept() {
    Settings::setDefaultDownloadFormat(Resources::DAILYMOTION, m_streamSelector->valueText());
    Settings::setSubtitlesEnabled(m_subtitleCheckBox->isChecked());
    Settings::setSubtitlesLanguage(subtitlesLanguage());
    Settings::setDefaultCategory(category());
    Dialog::accept();
}

void DailymotionDownloadDialog::list(const QString &videoId) {
    m_videoId = videoId;
    m_streamModel->list(videoId);

    if (m_subtitleCheckBox->isChecked()) {
        m_subtitleModel->list(videoId);
    }
}

void DailymotionDownloadDialog::onSubtitleCheckBoxToggled(bool enabled) {    
    if (enabled) {
        if (m_subtitleModel->status() == QDailymotion::ResourcesRequest::Null) {
            m_subtitleModel->list(m_videoId);
        }
    }
    else {
        m_subtitleModel->cancel();
    }
}

void DailymotionDownloadDialog::onStreamModelStatusChanged(QDailymotion::StreamsRequest::Status status) {
    switch (status) {
    case QDailymotion::StreamsRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::StreamsRequest::Ready:
        if (m_streamModel->rowCount() > 0) {
            m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                                   Settings::defaultDownloadFormat(Resources::DAILYMOTION))));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("No streams available"));
        }
        
        break;
    case QDailymotion::StreamsRequest::Failed:
        QMessageBox::critical(this, tr("Error"), tr("No streams available"));
        break;
    default:
        break;
    }
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_streamModel->rowCount() > 0);
    hideProgressIndicator();
}

void DailymotionDownloadDialog::onSubtitleModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Ready:
        if (m_subtitleModel->rowCount() > 0) {
            m_subtitleSelector->setCurrentIndex(0);
        }
        
        break;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::information(this, tr("Error"), tr("No subtitles available"));
        break;
    default:
        break;
    }
    
    m_subtitleCheckBox->setChecked(m_subtitleModel->rowCount() > 0);
    m_subtitleCheckBox->setEnabled(m_subtitleModel->rowCount() > 0);
    hideProgressIndicator();
}
