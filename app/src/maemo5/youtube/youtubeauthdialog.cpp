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

#include "youtubeauthdialog.h"
#include "logger.h"
#include "youtube.h"
#include "webview.h"
#include <QVBoxLayout>

YouTubeAuthDialog::YouTubeAuthDialog(QWidget *parent) :
    Dialog(parent),
    m_view(new WebView(this)),
    m_layout(new QVBoxLayout(this))
{
    setWindowTitle(tr("Authentication"));
    setMinimumHeight(360);
    
    m_layout->addWidget(m_view);
    
    connect(m_view, SIGNAL(titleChanged(QString)), this, SLOT(onWebViewTitleChanged(QString)));
    connect(m_view, SIGNAL(loadStarted()), this, SLOT(showProgressIndicator()));
    connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(hideProgressIndicator()));
}

QString YouTubeAuthDialog::code() const {
    return m_code;
}

void YouTubeAuthDialog::setCode(const QString &code) {
    m_code = code;
}

void YouTubeAuthDialog::login() {
    setCode(QString());
    m_view->setUrl(YouTube::authUrl());
}

void YouTubeAuthDialog::onWebViewTitleChanged(const QString &title) {
    Logger::log("DailymotionAuthDialog::onWebViewTitleChanged(): " + title);
    
    if (title.contains("code=")) {
        setCode(title.section("code=", 1, 1).section('&', 0, 0));
        accept();
    }
}
