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

#ifndef VIMEODOWNLOADDIALOG_H
#define VIMEODOWNLOADDIALOG_H

#include "dialog.h"
#include "vimeostreammodel.h"
#include "vimeosubtitlemodel.h"

class CategoryNameModel;
class ValueSelector;
class QScrollArea;
class QCheckBox;
class QLineEdit;
class QDialogButtonBox;
class QHBoxLayout;

class VimeoDownloadDialog : public Dialog
{
    Q_OBJECT

    Q_PROPERTY(QString videoId READ videoId)
    Q_PROPERTY(QString streamId READ streamId)
    Q_PROPERTY(QString subtitlesLanguage READ subtitlesLanguage)
    Q_PROPERTY(QString category READ category)
    Q_PROPERTY(QString customCommand READ customCommand)
    Q_PROPERTY(bool customCommandOverrideEnabled READ customCommandOverrideEnabled)
    
public:
    explicit VimeoDownloadDialog(QWidget *parent = 0);
    
    QString videoId() const;

    QString streamId() const;

    QString subtitlesLanguage() const;

    QString category() const;

    QString customCommand() const;
    bool customCommandOverrideEnabled() const;

public Q_SLOTS:
    virtual void accept();

    void list(const QString &videoId);

private Q_SLOTS:
    void onSubtitleCheckBoxToggled(bool enabled);
    void onStreamModelStatusChanged(QVimeo::StreamsRequest::Status status);
    void onSubtitleModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    
private:
    VimeoStreamModel *m_streamModel;
    VimeoSubtitleModel *m_subtitleModel;
    CategoryNameModel *m_categoryModel;
    
    QScrollArea *m_scrollArea;
    QCheckBox *m_subtitleCheckBox;
    QCheckBox *m_commandCheckBox;
    QLineEdit *m_commandEdit;
    ValueSelector *m_streamSelector;
    ValueSelector *m_subtitleSelector;
    ValueSelector *m_categorySelector;
    QDialogButtonBox *m_buttonBox;
    QHBoxLayout *m_layout;

    QString m_videoId;
};

#endif // VIMEODOWNLOADDIALOG_H
