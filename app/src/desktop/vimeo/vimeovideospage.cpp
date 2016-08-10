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

#include "vimeovideospage.h"
#include "imagecache.h"
#include "resources.h"
#include "transfers.h"
#include "videodelegate.h"
#include "vimeodownloaddialog.h"
#include <QListView>
#include <QMenu>
#include <QVBoxLayout>

VimeoVideosPage::VimeoVideosPage(QWidget *parent) :
    Page(parent),
    m_model(new VimeoVideoModel(this)),
    m_cache(new ImageCache),
    m_delegate(new VideoDelegate(m_cache, VimeoVideoModel::DateRole, VimeoVideoModel::DurationRole,
                                 VimeoVideoModel::ThumbnailUrlRole, VimeoVideoModel::TitleRole,
                                 VimeoVideoModel::UsernameRole, this)),
    m_view(new QListView(this)),
    m_layout(new QVBoxLayout(this))
{
    m_view->setModel(m_model);
    m_view->setItemDelegate(m_delegate);
    m_view->setContextMenuPolicy(Qt::CustomContextMenu);

    m_layout->addWidget(m_view);
    m_layout->setContentsMargins(0, 0, 0, 0);

    connect(m_model, SIGNAL(statusChanged(QVimeo::ResourcesRequest::Status)),
            this, SLOT(onModelStatusChanged(QVimeo::ResourcesRequest::Status)));
    connect(m_cache, SIGNAL(imageReady()), this, SLOT(onImageReady()));
    connect(m_delegate, SIGNAL(thumbnailClicked(QModelIndex)), this, SLOT(playVideo(QModelIndex)));
    connect(m_view, SIGNAL(clicked(QModelIndex)), this, SLOT(showVideo(QModelIndex)));
    connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(showContextMenu(QPoint)));
}

VimeoVideosPage::~VimeoVideosPage() {
    delete m_cache;
}

VimeoVideosPage::Status VimeoVideosPage::status() const {
    return m_status;
}

void VimeoVideosPage::setStatus(VimeoVideosPage::Status s) {
    if (s != status()) {
        m_status = s;
        emit statusChanged(s);
    }
}

void VimeoVideosPage::cancel() {
    m_model->cancel();
}

void VimeoVideosPage::reload() {
    m_model->reload();
}

void VimeoVideosPage::list(const QString &resourcePath, const QVariantMap &filters) {
    m_model->list(resourcePath, filters);
}

void VimeoVideosPage::downloadVideo(const QModelIndex &index) {
    const QString videoId = index.data(VimeoVideoModel::IdRole).toString();
    const QString title = index.data(VimeoVideoModel::TitleRole).toString();
    VimeoDownloadDialog dialog(this);
    dialog.list(videoId);

    if (dialog.exec() == QDialog::Accepted) {
        Transfers::instance()->addDownloadTransfer(Resources::VIMEO, videoId, dialog.streamId(),
                                                   QUrl(), title, dialog.category(), dialog.subtitlesLanguage(),
                                                   dialog.customCommand(), dialog.customCommandOverrideEnabled());
    }
}

void VimeoVideosPage::playVideo(const QModelIndex &) {

}

void VimeoVideosPage::showVideo(const QModelIndex &) {

}

void VimeoVideosPage::showContextMenu(const QPoint &pos) {
    if (!m_view->currentIndex().isValid()) {
        return;
    }

    QMenu menu(this);
    QAction *downloadAction = menu.addAction(tr("Download"));

    QAction *action = menu.exec(m_view->mapToGlobal(pos));

    if (action == downloadAction) {
        downloadVideo(m_view->currentIndex());
    }
}

void VimeoVideosPage::onImageReady() {
    m_view->viewport()->update(m_view->viewport()->rect());
}

void VimeoVideosPage::onModelStatusChanged(QVimeo::ResourcesRequest::Status status) {
    switch (status) {
    case QVimeo::ResourcesRequest::Loading:
        setStatus(Loading);
        break;
    default:
        setStatus(Ready);
        break;
    }
}
