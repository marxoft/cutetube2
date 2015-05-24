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

#include "dailymotionuserwindow.h"
#include "banner.h"
#include "dailymotion.h"
#include "dailymotionplaylistswindow.h"
#include "dailymotionplaylistwindow.h"
#include "dailymotionuserswindow.h"
#include "dailymotionvideoswindow.h"
#include "dailymotionvideowindow.h"
#include "definitions.h"
#include "listview.h"
#include "navdelegate.h"
#include "resources.h"
#include "settings.h"
#include "textbrowser.h"
#include "utils.h"
#include <qdailymotion/urls.h>
#include <QScrollArea>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QStringListModel>
#include <QDesktopServices>

DailymotionUserWindow::DailymotionUserWindow(const QString &id, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new DailymotionUser(this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Favourites") << tr("Playlists")
                                               << tr("Subscriptions"), this)),
    m_banner(new Banner(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_subscribeButton(new QPushButton(tr("Subscribe"), this))
{
    loadBaseUi();
    connect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onUserStatusChanged(QDailymotion::ResourcesRequest::Status)));
            
    m_user->loadUser(id);
}

DailymotionUserWindow::DailymotionUserWindow(const DailymotionUser *user, StackedWindow *parent) :
    StackedWindow(parent),
    m_user(new DailymotionUser(user, this)),
    m_model(new QStringListModel(QStringList() << tr("Videos") << tr("Favourites") << tr("Playlists")
                                               << tr("Subscriptions"), this)),
    m_banner(new Banner(this)),
    m_view(new ListView(this)),
    m_scrollArea(new QScrollArea(this)),
    m_titleLabel(new QLabel(this)),
    m_statsLabel(new QLabel(this)),
    m_descriptionLabel(new TextBrowser(this)),
    m_subscribeButton(new QPushButton(this))
{
    loadBaseUi();
    loadUserUi();
    connect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onUserUpdateStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(subscribedChanged()), this, SLOT(onUserSubscribedChanged())); 
    
    if ((!m_user->isSubscribed()) && (!Dailymotion::instance()->userId().isEmpty())) {
        m_user->checkIfSubscribed();
    }
}

void DailymotionUserWindow::loadBaseUi() {
    setWindowTitle(tr("User"));
    setCentralWidget(new QWidget);
    
    m_view->setModel(m_model);
    m_view->setItemDelegate(new NavDelegate(m_view));
    
    m_banner->setFixedSize(400, 100);
    
    m_statsLabel->setStyleSheet("font-size: 18px");
    
    m_subscribeButton->setEnabled(false);
    
    QWidget *scrollWidget = new QWidget(m_scrollArea);
    QGridLayout *grid = new QGridLayout(scrollWidget);
    grid->addWidget(m_banner, 0, 0, 1, 2, Qt::AlignLeft);
    grid->addWidget(m_titleLabel, 1, 0, Qt::AlignLeft | Qt::AlignTop);
    grid->addWidget(m_statsLabel, 1, 1, Qt::AlignRight | Qt::AlignTop);
    grid->addWidget(m_subscribeButton, 2, 0, Qt::AlignLeft);
    grid->addWidget(m_descriptionLabel, 3, 0, 1, 2, Qt::AlignTop);
    grid->setRowStretch(3, 1);
    grid->setColumnStretch(0, 1);
    grid->setContentsMargins(0, 0, 0, 0);
    m_scrollArea->setWidget(scrollWidget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFixedWidth(410);
    
    m_layout = new QHBoxLayout(centralWidget());
    m_layout->addWidget(m_scrollArea);
    m_layout->addWidget(m_view);
    m_layout->setStretch(1, 1);
    m_layout->setContentsMargins(0, 0, 0, 0);
    
    connect(m_view, SIGNAL(activated(QModelIndex)), this, SLOT(showWindow(QModelIndex)));
    connect(m_descriptionLabel, SIGNAL(anchorClicked(QUrl)), this, SLOT(showResource(QUrl)));
    connect(m_subscribeButton, SIGNAL(clicked()), this, SLOT(setUserSubscription()));
}

void DailymotionUserWindow::loadUserUi() {
    m_subscribeButton->setEnabled((!Dailymotion::instance()->userId().isEmpty())
                                  && (m_user->id() != Dailymotion::instance()->userId())
                                  && (Dailymotion::instance()->hasScope(QDailymotion::MANAGE_SUBSCRIPTIONS_SCOPE)));
    m_subscribeButton->setText(m_user->isSubscribed() ? tr("Unsubscribe") : tr("Subscribe"));
    m_titleLabel->setText(m_titleLabel->fontMetrics().elidedText(m_user->username(), Qt::ElideRight, 250));
    m_statsLabel->setText(tr("%1 %2\n%3 %4").arg(Utils::formatLargeNumber(m_user->subscriberCount()))
                                            .arg(tr("subscribers"))
                                            .arg(Utils::formatLargeNumber(m_user->viewCount()))
                                            .arg(tr("views")));
    m_descriptionLabel->setHtml(Utils::toRichText(m_user->description()));
    m_banner->setSource(m_user->bannerUrl());
    m_banner->setAvatarSource(m_user->thumbnailUrl());
}

void DailymotionUserWindow::showFavourites() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    filters["family_filter"] = Settings::instance()->safeSearchEnabled();
    
    DailymotionVideosWindow *window = new DailymotionVideosWindow(this);
    window->setWindowTitle(tr("%1's favourites").arg(m_user->username()));
    window->list(QString("/user/%1/favorites").arg(m_user->id()), filters);
    window->show();
}

void DailymotionUserWindow::showPlaylists() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    filters["family_filter"] = Settings::instance()->safeSearchEnabled();
    
    DailymotionPlaylistsWindow *window = new DailymotionPlaylistsWindow(this);
    window->setWindowTitle(tr("%1's playlists").arg(m_user->username()));
    window->list(QString("/user/%1/playlists").arg(m_user->id()), filters);
    window->show();
}

void DailymotionUserWindow::showSubscriptions() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    filters["family_filter"] = Settings::instance()->safeSearchEnabled();
    
    DailymotionUsersWindow *window = new DailymotionUsersWindow(this);
    window->setWindowTitle(tr("%1's subscriptions").arg(m_user->username()));
    window->list(QString("/user/%1/following").arg(m_user->id()), filters);
    window->show();
}

void DailymotionUserWindow::showUploads() {
    QVariantMap filters;
    filters["limit"] = MAX_RESULTS;
    filters["family_filter"] = Settings::instance()->safeSearchEnabled();
    
    DailymotionVideosWindow *window = new DailymotionVideosWindow(this);
    window->setWindowTitle(tr("%1's videos").arg(m_user->username()));
    window->list(QString("/user/%1/videos").arg(m_user->id()), filters);
    window->show();
}

void DailymotionUserWindow::showResource(const QUrl &url) {
    QVariantMap resource = Resources::getResourceFromUrl(url.toString());
    
    if (resource.value("service") != Resources::DAILYMOTION) {
        QDesktopServices::openUrl(url);
        return;
    }
    
    if (resource.value("type") == Resources::PLAYLIST) {
        DailymotionPlaylistWindow *window = new DailymotionPlaylistWindow(resource.value("id").toString(), this);
        window->show();
    }
    else if (resource.value("type") == Resources::USER) {
        DailymotionUserWindow *window = new DailymotionUserWindow(resource.value("id").toString(), this);
        window->show();
    }
    else {
        DailymotionVideoWindow *window = new DailymotionVideoWindow(resource.value("id").toString(), this);
        window->show();
    }
}

void DailymotionUserWindow::showWindow(const QModelIndex &index) {
    if (isBusy()) {
        return;
    }
    
    switch (index.row()) {
    case 0:
        showUploads();
        break;
    case 1:
        showFavourites();
        break;
    case 2:
        showPlaylists();
        break;
    case 3:
        showSubscriptions();
        break;
    default:
        break;
    }
}

void DailymotionUserWindow::setUserSubscription() {
    if (isBusy()) {
        return;
    }
    
    if (m_user->isSubscribed()) {
        m_user->unsubscribe();
    }
    else {
        m_user->subscribe();
    }
}

void DailymotionUserWindow::onUserStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Ready:
        loadUserUi();
        break;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
    disconnect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
               this, SLOT(onUserStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(statusChanged(QDailymotion::ResourcesRequest::Status)),
            this, SLOT(onUserUpdateStatusChanged(QDailymotion::ResourcesRequest::Status)));
    connect(m_user, SIGNAL(subscribedChanged()), this, SLOT(onUserSubscribedChanged())); 
    
    if ((!m_user->isSubscribed()) && (!Dailymotion::instance()->userId().isEmpty())) {
        m_user->checkIfSubscribed();
    }
}

void DailymotionUserWindow::onUserUpdateStatusChanged(QDailymotion::ResourcesRequest::Status status) {
    switch (status) {
    case QDailymotion::ResourcesRequest::Loading:
        showProgressIndicator();
        return;
    case QDailymotion::ResourcesRequest::Failed:
        QMessageBox::critical(this, tr("Error"), m_user->errorString());
        break;
    default:
        break;
    }
    
    hideProgressIndicator();
}

void DailymotionUserWindow::onUserSubscribedChanged() {
    m_subscribeButton->setText(m_user->isSubscribed() ? tr("Unsubscribe") : tr("Subscribe"));
}
