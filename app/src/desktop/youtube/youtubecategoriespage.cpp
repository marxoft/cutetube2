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

#include "youtubecategoriespage.h"
#include "definitions.h"
#include "youtubevideospage.h"
#include "pagestack.h"
#include <QListView>
#include <QMessageBox>
#include <QVBoxLayout>

YouTubeCategoriesPage::YouTubeCategoriesPage(QWidget *parent) :
    Page(parent),
    m_model(new YouTubeCategoryModel(this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_view->setModel(m_model);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QYouTube::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QYouTube::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideos(QModelIndex)));
}

YouTubeCategoriesPage::Status YouTubeCategoriesPage::status() const {
    return m_status;
}

void YouTubeCategoriesPage::setStatus(YouTubeCategoriesPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString YouTubeCategoriesPage::statusText() const {
    return m_statusText;
}

void YouTubeCategoriesPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void YouTubeCategoriesPage::cancel() {
    m_model->cancel();
}

void YouTubeCategoriesPage::reload() {
    m_model->reload();
}

void YouTubeCategoriesPage::list(const QString &resourcePath, const QStringList &part, const QVariantMap &filters,
                                 const QVariantMap &params) {
    m_model->list(resourcePath, part, filters, params);
}

void YouTubeCategoriesPage::showVideos(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["chart"] = "mostPopular";
        QVariantMap params;
        params["videoCategoryId"] = index.data(YouTubeCategoryModel::ValueRole);
        params["maxResults"] = MAX_RESULTS;
        YouTubeVideosPage *page = new YouTubeVideosPage(this);
        page->setWindowTitle(index.data(YouTubeCategoryModel::NameRole).toString());
        page->list("/videos", QStringList() << "snippet" << "contentDetails" << "statistics", filters, params);
        s->push(page);
    }
}

void YouTubeCategoriesPage::onModelStatusChanged(QYouTube::ResourcesRequest::Status status) {
    switch (status) {
    case QYouTube::ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading categories"));
        return;
    case QYouTube::ResourcesRequest::Ready:
        setStatusText(m_model->rowCount() > 0 ? tr("Ready") : tr("No categories found"));
        break;
    case QYouTube::ResourcesRequest::Canceled:
        setStatusText(tr("Canceled"));
        break;
    case QYouTube::ResourcesRequest::Failed:
        setStatusText(m_model->errorString());
        QMessageBox::critical(this, tr("Error"), m_model->errorString());
        break;
    default:        
        break;
    }
    
    setStatus(Ready);
}
