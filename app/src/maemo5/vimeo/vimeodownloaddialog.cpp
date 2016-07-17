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

#include "vimeodownloaddialog.h"
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

VimeoDownloadDialog::VimeoDownloadDialog(QWidget *parent) :
    Dialog(parent),
    m_streamModel(new VimeoStreamModel(this)),
    m_subtitleModel(new VimeoSubtitleModel(this)),
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
    
    connect(m_streamModel, SIGNAL(statusChanged(QVimeo::StreamsRequest::Status)), this,
            SLOT(onStreamModelStatusChanged(QVimeo::StreamsRequest::Status)));
    connect(m_subtitleModel, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)), this,
            SLOT(onSubtitleModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_subtitleCheckBox, SIGNAL(toggled(bool)), this, SLOT(onSubtitleCheckBoxToggled(bool)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));    
}

QString VimeoDownloadDialog::videoId() const {
    return m_videoId;
}

QString VimeoDownloadDialog::streamId() const {
    return m_streamSelector->currentValue().toMap().value("id").toString();
}

QString VimeoDownloadDialog::subtitlesLanguage() const {
    return m_subtitleCheckBox->isChecked()
           ? m_subtitleSelector->currentValue().toMap().value("id").toString()
           : QString();
}

QString VimeoDownloadDialog::category() const {
    return m_categorySelector->valueText();
}

QString VimeoDownloadDialog::customCommand() const {
    return m_commandEdit->text();
}

bool VimeoDownloadDialog::customCommandOverrideEnabled() const {
    return m_commandCheckBox->isChecked();
}

void VimeoDownloadDialog::accept() {
    Settings::setDefaultDownloadFormat(Resources::VIMEO, m_streamSelector->valueText());
    Settings::setSubtitlesEnabled(m_subtitleCheckBox->isChecked());
    Settings::setSubtitlesLanguage(subtitlesLanguage());
    Settings::setDefaultCategory(category());
    Dialog::accept();
}

void VimeoDownloadDialog::list(const QString &videoId) {
    m_videoId = videoId;
    m_streamModel->list(videoId);

    if (m_subtitleCheckBox->isChecked()) {
        m_subtitleModel->list(videoId);
    }
}

void VimeoDownloadDialog::onSubtitleCheckBoxToggled(bool enabled) {    
    if (enabled) {
        if (m_subtitleModel->status() == QVimeo::ResourcesRequest::Null) {
            m_subtitleModel->list(m_videoId);
        }
    }
    else {
        m_subtitleModel->cancel();
    }
}

void VimeoDownloadDialog::onStreamModelStatusChanged(QVimeo::StreamsRequest::Status status) {
    switch (status) {
    case QVimeo::StreamsRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::StreamsRequest::Ready:
        if (m_streamModel->rowCount() > 0) {
            m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                                   Settings::defaultDownloadFormat(Resources::VIMEO))));
        }
        else {
            QMessageBox::critical(this, tr("Error"), tr("No streams available"));
        }
        
        break;
    case QVimeo::StreamsRequest::Failed:
        QMessageBox::critical(this, tr("Error"), tr("No streams available"));
        break;
    default:
        break;
    }
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_streamModel->rowCount() > 0);
    hideProgressIndicator();
}

void VimeoDownloadDialog::onSubtitleModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        if (m_subtitleModel->rowCount() > 0) {
            m_subtitleSelector->setCurrentIndex(0);
        }
        
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::information(this, tr("Error"), tr("No subtitles available"));
        break;
    default:
        break;
    }
    
    m_subtitleCheckBox->setChecked(m_subtitleModel->rowCount() > 0);
    m_subtitleCheckBox->setEnabled(m_subtitleModel->rowCount() > 0);
    hideProgressIndicator();
}
