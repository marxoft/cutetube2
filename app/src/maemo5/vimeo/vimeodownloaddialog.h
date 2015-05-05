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

#ifndef VIMEODOWNLOADDIALOG_H
#define VIMEODOWNLOADDIALOG_H

#include "dialog.h"
#include "vimeostreammodel.h"
#include "vimeosubtitlemodel.h"

class CategoryNameModel;
class ValueSelector;
class QScrollArea;
class QCheckBox;
class QDialogButtonBox;
class QHBoxLayout;

class VimeoDownloadDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit VimeoDownloadDialog(const QString &resourceId, const QString &title, QWidget *parent = 0);
    
protected:
    void showEvent(QShowEvent *e);
    
private Q_SLOTS:
    void onCategoryChanged();
    void onStreamChanged();
    void onSubtitlesChanged();
    void onSubtitleCheckBoxToggled(bool enabled);
    void onStreamModelStatusChanged(QVimeo::StreamsRequest::Status status);
    void onSubtitleModelStatusChanged(QVimeo::ResourcesRequest::Status status);
    
    void addDownload();
    
private:
    QString m_id;
    QString m_title;
    VimeoStreamModel *m_streamModel;
    VimeoSubtitleModel *m_subtitleModel;
    CategoryNameModel *m_categoryModel;
    
    QScrollArea *m_scrollArea;
    QCheckBox *m_subtitleCheckBox;
    QCheckBox *m_audioCheckBox;
    ValueSelector *m_streamSelector;
    ValueSelector *m_subtitleSelector;
    ValueSelector *m_categorySelector;
    QDialogButtonBox *m_buttonBox;
    QHBoxLayout *m_layout;
};

#endif // VIMEODOWNLOADDIALOG_H
