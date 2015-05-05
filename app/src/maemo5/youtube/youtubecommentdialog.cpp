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

#include "youtubecommentdialog.h"
#include <QTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

YouTubeCommentDialog::YouTubeCommentDialog(const QString &videoId, const QString &parentId, QWidget *parent) :
    Dialog(parent),
    m_comment(0),
    m_videoId(videoId),
    m_parentId(parentId),
    m_edit(new QTextEdit(this)),
    m_buttonBox(new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this)),
    m_layout(new QGridLayout(this))
{
    setWindowTitle(tr("Add comment"));
    
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    
    m_layout->addWidget(new QLabel(tr("Comment"), this), 0, 0);
    m_layout->addWidget(m_edit, 1, 0);
    m_layout->addWidget(m_buttonBox, 1, 1, 2, 1);
    m_layout->setRowStretch(1, 1);
    
    connect(m_edit, SIGNAL(textChanged()), this, SLOT(onCommentChanged()));
    connect(m_buttonBox, SIGNAL(accepted()), this, SLOT(addComment()));
    connect(m_buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}

void YouTubeCommentDialog::addComment() {
    if (isBusy()) {
        return;
    }
    
    if (!m_comment) {
        m_comment = new YouTubeComment(this);
        connect(m_comment, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
                this, SLOT(onCommentStatusChanged(QYouTube::ResourcesRequest::Status)));
    }
    
    QVariantMap comment;
    QVariantMap snippet;
    snippet["videoId"] = m_videoId;
    snippet["textDisplay"] = m_edit->toPlainText();
    
    if (!m_parentId.isEmpty()) {
        snippet["parentId"] = m_parentId;
    }
    
    comment["snippet"] = snippet;
    m_comment->loadComment(comment);
    m_comment->addComment();
}

void YouTubeCommentDialog::onCommentChanged() {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_edit->toPlainText().isEmpty());
}

void YouTubeCommentDialog::onCommentStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QYouTube::ResourcesRequest::Ready:
        accept();
        return;
    case QYouTube::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_comment->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}
