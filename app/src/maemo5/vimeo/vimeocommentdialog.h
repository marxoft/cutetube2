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

#ifndef VIMEOCOMMENTDIALOG_H
#define VIMEOCOMMENTDIALOG_H

#include "dialog.h"
#include "vimeocomment.h"

class QTextEdit;
class QDialogButtonBox;
class QGridLayout;

class VimeoCommentDialog : public Dialog
{
    Q_OBJECT
    
public:
    explicit VimeoCommentDialog(const QString &videoId, QWidget *parent = 0);
    
private Q_SLOTS:
    void addComment();
    
    void onCommentChanged();
    void onCommentStatusChanged(QVimeo::ResourcesRequest::Status);
    
private:
    VimeoComment *m_comment;
    QString m_id;
    
    QTextEdit *m_edit;
    QDialogButtonBox *m_buttonBox;
    QGridLayout *m_layout;
};

#endif // VIMEOCOMMENTDIALOG_H
