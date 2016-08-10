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

#ifndef PLUGINDOWNLOADDIALOG_H
#define PLUGINDOWNLOADDIALOG_H

#include "pluginstreammodel.h"
#include "pluginsubtitlemodel.h"
#include <QDialog>

class CategoryNameModel;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QFormLayout;
class QLineEdit;

class PluginDownloadDialog : public QDialog
{
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId)
    Q_PROPERTY(QString streamId READ streamId)
    Q_PROPERTY(QString subtitlesLanguage READ subtitlesLanguage)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(QString customCommand READ customCommand)
    Q_PROPERTY(bool customCommandOverrideEnabled READ customCommandOverrideEnabled)

public:
    explicit PluginDownloadDialog(const QString &service, QWidget *parent = 0);

    QString videoId() const;
    
    QString streamId() const;
    
    QString subtitlesLanguage() const;

    QString category() const;

    QString customCommand() const;
    bool customCommandOverrideEnabled() const;

public Q_SLOTS:
    virtual void accept();

    void list(const QString &videoId, bool listStreams = true, bool listSubtitles = false);

private Q_SLOTS:
    void setSubtitlesEnabled(bool enabled);
    
    void onStreamModelStatusChanged(ResourcesRequest::Status status);
    void onSubtitleModelStatusChanged(ResourcesRequest::Status status);

private:
    PluginStreamModel *m_streamModel;
    PluginSubtitleModel *m_subtitleModel;
    CategoryNameModel *m_categoryModel;

    QComboBox *m_streamSelector;
    QComboBox *m_subtitleSelector;
    QComboBox *m_categorySelector;

    QCheckBox *m_subtitleCheckBox;
    QCheckBox *m_commandCheckBox;
    
    QLineEdit *m_commandEdit;

    QDialogButtonBox *m_buttonBox;

    QFormLayout *m_layout;

    QString m_videoId;
};

#endif // PLUGINDOWNLOADDIALOG_H
