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

#include "dailymotioncategoriespage.h"
#include "dailymotionvideospage.h"
#include "definitions.h"
#include "pagestack.h"
#include <QListView>
#include <QVBoxLayout>

DailymotionCategoriesPage::DailymotionCategoriesPage(QWidget *parent) :
    Page(parent),
    m_model(new DailymotionCategoryModel(this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_view->setModel(m_model);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideos(QModelIndex)));
}

DailymotionCategoriesPage::Status DailymotionCategoriesPage::status() const {
    return m_status;
}

void DailymotionCategoriesPage::setStatus(DailymotionCategoriesPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

QString DailymotionCategoriesPage::statusText() const {
    return m_statusText;
}

void DailymotionCategoriesPage::setStatusText(const QString &text) {
    if (text != statusText()) {
        m_statusText = text;
        emit statusTextChanged(text);
    }
}

void DailymotionCategoriesPage::cancel() {
    m_model->cancel();
}

void DailymotionCategoriesPage::reload() {
    m_model->reload();
}

void DailymotionCategoriesPage::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void DailymotionCategoriesPage::showVideos(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["limit"] = MAX_RESULTS;
        DailymotionVideosPage *page = new DailymotionVideosPage(this);
        page->setWindowTitle(index.data(DailymotionCategoryModel::NameRole).toString());
        page->list(QString("/channel/%1/videos").arg(index.data(DailymotionCategoryModel::ValueRole).toString()),
                   filters);
        s->push(page);
    }
}

void DailymotionCategoriesPage::onModelStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        setStatus(Loading);
        setStatusText(tr("Loading categories"));
        return;
    case QDailymotion::ResourcesRequest::Ready:
        setStatusText(m_model->rowCount() > 0 ? tr("Ready") : tr("No categories found"));
        break;
    case QDailymotion::ResourcesRequest::Canceled:
        setStatusText(tr("Canceled"));
        break;
    case QDailymotion::ResourcesRequest::Failed:
        setStatusText(m_model->errorString());
        break;
    default:        
        break;
    }
    
    setStatus(Ready);
}
