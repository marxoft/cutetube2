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

#include "vimeocategoriespage.h"
#include "definitions.h"
#include "vimeovideospage.h"
#include "pagestack.h"
#include <QListView>
#include <QVBoxLayout>

VimeoCategoriesPage::VimeoCategoriesPage(QWidget *parent) :
    Page(parent),
    m_model(new VimeoCategoryModel(this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this)),
    m_status(Null)
{
    m_view->setModel(m_model);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideos(QModelIndex)));
}

VimeoCategoriesPage::Status VimeoCategoriesPage::status() const {
    return m_status;
}

void VimeoCategoriesPage::setStatus(VimeoCategoriesPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void VimeoCategoriesPage::cancel() {
    m_model->cancel();
}

void VimeoCategoriesPage::reload() {
    m_model->reload();
}

void VimeoCategoriesPage::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void VimeoCategoriesPage::showVideos(const QModelIndex &index) {
    if (PageStack *s = stack()) {
        QVariantMap filters;
        filters["per_page"] = MAX_RESULTS;
        VimeoVideosPage *page = new VimeoVideosPage(this);
        page->setWindowTitle(index.data(VimeoCategoryModel::NameRole).toString());
        page->list(QString("/categories/%1/videos").arg(index.data(VimeoCategoryModel::ValueRole).toString()),
                   filters);
        s->push(page);
    }
}

void VimeoCategoriesPage::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
