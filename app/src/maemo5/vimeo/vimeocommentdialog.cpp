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

#include "vimeocommentdialog.h"
#include <QTextEdit>
#include <QLabel>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>

VimeoCommentDialog::VimeoCommentDialog(const QString &videoId, QWidget *parent) :
    Dialog(parent),
    m_comment(0),
    m_id(videoId),
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

void VimeoCommentDialog::addComment() {
    if (isBusy()) {
        return;
    }
    
    if (!m_comment) {
        m_comment = new VimeoComment(this);
        connect(m_comment, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
                this, SLOT(onCommentStatusChanged(QVimeo::ResourcesRequest::Status)));
    }
    
    QVariantMap comment;
    comment["uri"] = QString("/videos/%1/comments").arg(m_id);
    comment["text"] = m_edit->toPlainText();
    m_comment->loadComment(comment);
    m_comment->addComment();
}

void VimeoCommentDialog::onCommentChanged() {
    m_buttonBox->button(QDialogButtonBox::Ok)->setEnabled(!m_edit->toPlainText().isEmpty());
}

void VimeoCommentDialog::onCommentStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QVimeo::ResourcesRequest::Ready:
        accept();
        return;
    case QVimeo::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_comment->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}
