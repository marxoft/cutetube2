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

#include "youtubeauthdialog.h"
#include "youtube.h"
#include "webview.h"
#include <QVBoxLayout>
#ifdef CUTETUBE_DEBUG
#include <QDebug>
#endif

YouTubeAuthDialog::YouTubeAuthDialog(QWidget *parent) :
    Dialog(parent),
    m_view(new WebView(this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle(tr("Authentication"));
    
    m_layout->addWidget(m_view);
    
    connect(m_view, SIGNAL(titleChanged(QString)), this, SLOT(onWebViewTitleChanged(QString)));
    connect(m_view, SIGNAL(loadStarted()), this, SLOT(showProgressIndicator()));
    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(hideProgressIndicator()));
}

void YouTubeAuthDialog::showEvent(QShowEvent *e) {
    Dialog::showEvent(e);
    m_view->setUrl(YouTube::instance()->authUrl());
}

void YouTubeAuthDialog::onWebViewTitleChanged(const QString &title) {
#ifdef CUTETUBE_DEBUG
    qDebug() << "YouTubeAuthDialog::onWebViewTitleChanged" << title;
#endif
    if (title.contains("code=")) {
        emit codeReady(title.section("code=", 1, 1).section('&', 0, 0));
        accept();
    }
}
