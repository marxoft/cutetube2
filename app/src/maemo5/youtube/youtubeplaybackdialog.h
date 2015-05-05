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

#ifndef YOUTUBEPLAYBACKDIALOG_H
#define YOUTUBEPLAYBACKDIALOG_H

#include "dialog.h"
#include "youtubestreammodel.h"

class ValueSelector;
class QDialogButtonBox;
class QHBoxLayout;

class YouTubePlaybackDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit YouTubePlaybackDialog(const QString &resourceId, const QString &title, QWidget *parent = 0);
    
protected:
    void showEvent(QShowEvent *e);
    
private Q_SLOTS:
    void onModelStatusChanged(QYouTube::StreamsRequest::Status status);
    void onStreamChanged();
    
    void playVideo();
    
private:
    QString m_id;
    QString m_title;
    YouTubeStreamModel *m_model;
    
    ValueSelector *m_streamSelector;
    QDialogButtonBox *m_buttonBox;
    QHBoxLayout *m_layout;
};

#endif // YOUTUBEPLAYBACKDIALOG_H
