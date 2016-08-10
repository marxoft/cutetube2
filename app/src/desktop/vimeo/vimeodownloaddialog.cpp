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
#include <QCheckBox>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>

VimeoDownloadDialog::VimeoDownloadDialog(QWidget *parent) :
    QDialog(parent),
    m_streamModel(new VimeoStreamModel(this)),
    m_subtitleModel(new VimeoSubtitleModel(this)),
    m_categoryModel(new CategoryNameModel(this)),
    m_streamSelector(new QComboBox(this)),
    m_subtitleSelector(new QComboBox(this)),
    m_categorySelector(new QComboBox(this)),
    m_subtitleCheckBox(new QCheckBox(tr("Download &subtitles"), this)),
    m_commandCheckBox(new QCheckBox(tr("&Override global custom command"), this)),
    m_commandEdit(new QLineEdit(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this)),
    m_layout(new QFormLayout(this))
{
    setWindowTitle(tr("Download video"));

    m_streamSelector->setModel(m_streamModel);

    m_subtitleSelector->setModel(m_subtitleModel);

    m_categorySelector->setModel(m_categoryModel);

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    m_layout->addRow(tr("Video &format:"), m_streamSelector);
    m_layout->addRow(m_subtitleCheckBox);
    m_layout->addRow(tr("Subtitles &language:"), m_subtitleSelector);
    m_layout->addRow(tr("&Category:"), m_categorySelector);
    m_layout->addRow(tr("&Custom command:"), m_commandEdit);
    m_layout->addRow(m_commandCheckBox);
    m_layout->addRow(m_buttonBox);

    connect(m_streamModel, SIGNAL(statusChanged(QVimeo::StreamsRequest::Status)),
            this, SLOT(onStreamModelStatusChanged(QVimeo::StreamsRequest::Status)));
    connect(m_subtitleModel, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onSubtitleModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_subtitleCheckBox, SIGNAL(toggled(bool)), this, SLOT(setSubtitlesEnabled(bool)));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

QString VimeoDownloadDialog::videoId() const {
    return m_videoId;
}

QString VimeoDownloadDialog::streamId() const {
    return m_streamSelector->itemData(m_streamSelector->currentIndex()).toMap().value("id").toString();
}

QString VimeoDownloadDialog::subtitlesLanguage() const {
    return m_subtitleCheckBox->isChecked() ? m_subtitleSelector->currentText() : QString();
}

QString VimeoDownloadDialog::category() const {
    return m_categorySelector->currentText();
}

QString VimeoDownloadDialog::customCommand() const {
    return m_commandEdit->text();
}

bool VimeoDownloadDialog::customCommandOverrideEnabled() const {
    return m_commandCheckBox->isChecked();
}

void VimeoDownloadDialog::accept() {
    Settings::setDefaultDownloadFormat(Resources::VIMEO, m_streamSelector->currentText());
    Settings::setSubtitlesEnabled(m_subtitleCheckBox->isChecked());
    Settings::setSubtitlesLanguage(subtitlesLanguage());
    Settings::setDefaultCategory(category());
    QDialog::accept();
}

void VimeoDownloadDialog::list(const QString &videoId) {
    m_videoId = videoId;
    m_streamModel->list(videoId);

    if (m_subtitleCheckBox->isChecked()) {
        m_subtitleModel->list(videoId);
    }
    else {
        m_subtitleCheckBox->setChecked(Settings::subtitlesEnabled());
    }
}

void VimeoDownloadDialog::setSubtitlesEnabled(bool enabled) {
    if (enabled) {
        m_subtitleModel->list(m_videoId);
    }
    else {
        m_subtitleModel->cancel();
    }
}

void VimeoDownloadDialog::onStreamModelStatusChanged(QVimeo::StreamsRequest::Status status) {
    switch (status) {
    case QVimeo::StreamsRequest::Loading:
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        break;
    case QVimeo::StreamsRequest::Ready:
        m_streamSelector->setCurrentIndex(qMax(0, m_streamModel->match("name",
                                          Settings::defaultDownloadFormat(Resources::VIMEO))));
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_subtitleModel->status()
                                                              != QVimeo::ResourcesRequest::Loading);
        break;
    case QVimeo::StreamsRequest::Failed:
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        QMessageBox::critical(this, tr("Error"), m_streamModel->errorString());
        break;
    default:
        break;
    }
}

void VimeoDownloadDialog::onSubtitleModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {    
    case QVimeo::ResourcesRequest::Loading:
        m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
        return;
    case QVimeo::ResourcesRequest::Ready:
        m_subtitleSelector->setCurrentIndex(qMax(0, m_streamModel->match("name", Settings::subtitlesLanguage())));
        break;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_subtitleModel->errorString());
        break;
    default:
        break;
    }

    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(m_streamModel->status()
                                                          == QVimeo::StreamsRequest::Ready);
}
