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

#ifndef YOUTUBECOMMENTDIALOG_H
#define YOUTUBECOMMENTDIALOG_H

#include "dialog.h"
#include "youtubecomment.h"

class QTextEdit;
class QDialogButtonBox;
class QGridLayout;

class YouTubeCommentDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit YouTubeCommentDialog(const QString &videoId, const QString &parentId = QString(), QWidget *parent = 0);
    
private Q_SLOTS:
    void addComment();
    
    void onCommentChanged();
    void onCommentStatusChanged(QYouTube::ResourcesRequest::Status);
    
private:
    YouTubeComment *m_comment;
    QString m_videoId;
    QString m_parentId;
    
    QTextEdit *m_edit;
    QDialogButtonBox *m_buttonBox;
    QGridLayout *m_layout;
};

#endif // YOUTUBECOMMENTDIALOG_H
