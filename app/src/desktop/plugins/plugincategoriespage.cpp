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

#include "plugincategoriespage.h"
#include "pagestack.h"
#include "plugincategorymodel.h"
#include "pluginvideospage.h"
#include <QListView>
#include <QVBoxLayout>

PluginCategoriesPage::PluginCategoriesPage(const QString &service, QWidget *parent) :
    Page(parent),
    m_model(new PluginCategoryModel(this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_model->setService(service);

    m_view->setModel(m_model);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(ResourcesRequest::Status)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideos(QModelIndex)));
}

PluginCategoriesPage::Status PluginCategoriesPage::status() const {
    return m_status;
}

void PluginCategoriesPage::setStatus(PluginCategoriesPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void PluginCategoriesPage::cancel() {
    m_model->cancel();
}

void PluginCategoriesPage::reload() {
    m_model->reload();
}

void PluginCategoriesPage::list(const QString &resourceId) {
    m_model->list(resourceId);
}

void PluginCategoriesPage::search(const QString &query, const QString &order) {
    m_model->search(query, order);
}

void PluginCategoriesPage::showVideos(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        PluginVideosPage *page = new PluginVideosPage(m_model->service(), this);
        page->setWindowTitle(index.data(PluginCategoryModel::NameRole).toString());
        page->list(index.data(PluginCategoryModel::ValueRole).toMap().value("videosId").toString());
        s->push(page);
    }
}

void PluginCategoriesPage::onModelStatusChanged(ResourcesRequest::Status status) {
    switch (status) {
    case ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
