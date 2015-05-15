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

#include "transfers.h"
#include "definitions.h"
#include "settings.h"
#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QSettings>
#include <QDateTime>

Transfers* Transfers::self = 0;

Transfers::Transfers(QObject *parent) :
    QObject(parent),
    m_nam(new QNetworkAccessManager(this))
{
    if (!self) {
        self = this;
    }
    
    m_queueTimer.setSingleShot(true);
    m_queueTimer.setInterval(1000);
    
    connect(&m_queueTimer, SIGNAL(timeout()), this, SLOT(startNextTransfers()));
    connect(Settings::instance(), SIGNAL(maximumConcurrentTransfersChanged()),
            this, SLOT(onMaximumConcurrentTransfersChanged()));
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(storeTransfers()));
}

Transfers::~Transfers() {
    if (self == this) {
        self = 0;
    }
}

Transfers* Transfers::instance() {
    return self;
}

int Transfers::active() const {
    return m_active.size();
}

int Transfers::count() const {
    return m_transfers.size();
}

void Transfers::addDownloadTransfer(const QString &service, const QString &resourceId, const QString &streamId,
                                    const QString &title, const QString &category, const QString &subtitlesLanguage,
                                    bool convertToAudio) {
    Transfer *transfer = new Transfer(this);
    transfer->setNetworkAccessManager(m_nam);
    transfer->setId(QByteArray(QByteArray::number(QDateTime::currentMSecsSinceEpoch()) + "#"
                    + resourceId.toUtf8()).toBase64());
    transfer->setDownloadPath(Settings::instance()->downloadPath() + ".incomplete/" + transfer->id());
    transfer->setFileName(title + ".mp4");
    transfer->setCategory(category);
    transfer->setService(service);
    transfer->setResourceId(resourceId);
    transfer->setStreamId(streamId);
    transfer->setTitle(title);
    transfer->setConvertToAudio(convertToAudio);
    
    if (!subtitlesLanguage.isEmpty()) {
        transfer->setDownloadSubtitles(true);
        transfer->setSubtitlesLanguage(subtitlesLanguage);
    }
    
    connect(transfer, SIGNAL(statusChanged()), this, SLOT(onTransferStatusChanged()));
    
    m_transfers << transfer;
    emit countChanged(count());
    emit transferAdded(transfer);
    
    if (Settings::instance()->startTransfersAutomatically()) {
        transfer->queue();
    }
}

Transfer* Transfers::get(int i) const {
    if ((i >= 0) && (i < m_transfers.size())) {
        return m_transfers.at(i);
    }
    
    return 0;
}

Transfer* Transfers::get(const QString &id) const {
    foreach (Transfer *transfer, m_transfers) {
        if (transfer->id() == id) {
            return transfer;
        }
    }
    
    return 0;
}

bool Transfers::start() {
    foreach (Transfer *transfer, m_transfers) {
        transfer->queue();
    }
    
    return true;
}

bool Transfers::pause() {
    foreach (Transfer *transfer, m_transfers) {
        transfer->pause();
    }
    
    return true;
}

bool Transfers::start(const QString &id) {
    if (Transfer *transfer = get(id)) {
        transfer->queue();
        return true;
    }
    
    return false;
}

bool Transfers::pause(const QString &id) {
    if (Transfer *transfer = get(id)) {
        transfer->pause();
        return true;
    }
    
    return false;
}

bool Transfers::cancel(const QString &id) {
    if (Transfer *transfer = get(id)) {
        transfer->cancel();
        return true;
    }
    
    return false;
}

void Transfers::storeTransfers() {
    QSettings settings(STORAGE_PATH + "transfers.conf", QSettings::NativeFormat);
    settings.clear();
    
    foreach (Transfer *transfer, m_transfers) {
        settings.beginGroup(transfer->id());
        settings.setValue("downloadPath", transfer->downloadPath());
        settings.setValue("fileName", transfer->fileName());
        settings.setValue("category", transfer->category());
        settings.setValue("priority", Transfer::Priority(transfer->priority()));
        settings.setValue("size", transfer->size());
        settings.setValue("service", transfer->service());
        settings.setValue("resourceId", transfer->resourceId());
        settings.setValue("streamId", transfer->streamId());
        settings.setValue("title", transfer->title());
        settings.setValue("convertToAudio", transfer->convertToAudio());
        settings.setValue("downloadSubtitles", transfer->downloadSubtitles());
        settings.setValue("subtitlesLanguage", transfer->subtitlesLanguage());
        settings.endGroup();
    }
}

void Transfers::restoreTransfers() {
    QSettings settings(STORAGE_PATH + "transfers.conf", QSettings::NativeFormat);

    foreach (QString group, settings.childGroups()) {
        settings.beginGroup(group);
        Transfer *transfer = new Transfer(this);
        transfer->setId(group);
        transfer->setDownloadPath(settings.value("downloadPath").toString());
        transfer->setFileName(settings.value("fileName").toString());
        transfer->setCategory(settings.value("category").toString());
        transfer->setPriority(Transfer::Priority(settings.value("priority").toInt()));
        transfer->setSize(settings.value("size").toLongLong());
        transfer->setService(settings.value("service").toString());
        transfer->setResourceId(settings.value("resourceId").toString());
        transfer->setStreamId(settings.value("streamId").toString());
        transfer->setTitle(settings.value("title").toString());
        transfer->setConvertToAudio(settings.value("convertToAudio").toBool());
        transfer->setDownloadSubtitles(settings.value("downloadSubtitles").toBool());
        transfer->setSubtitlesLanguage(settings.value("subtitlesLanguage").toString());
        settings.endGroup();
        
        connect(transfer, SIGNAL(statusChanged()), this, SLOT(onTransferStatusChanged()));
    
        m_transfers << transfer;
        emit countChanged(count());
        emit transferAdded(transfer);
    
        if (Settings::instance()->startTransfersAutomatically()) {
            transfer->queue();
        }
    }
}

void Transfers::getNextTransfers() {
    const int max = Settings::instance()->maximumConcurrentTransfers();
    
    for (int priority = Transfer::HighPriority; priority <= Transfer::LowPriority; priority++) {
        foreach (Transfer *transfer, m_transfers) {
            if ((transfer->priority() == priority) && (transfer->status() == Transfer::Queued)) {
                if (active() < max) {
                    addActiveTransfer(transfer);
                }
                else {
                    return;
                }
            }
        }
    }
}

void Transfers::startNextTransfers() {
    getNextTransfers();
    
    foreach (Transfer *transfer, m_active) {
        transfer->start();
    }
}

void Transfers::removeTransfer(Transfer *transfer) {
    removeActiveTransfer(transfer);
    m_transfers.removeOne(transfer);
    transfer->deleteLater();
    emit countChanged(count());
}

void Transfers::addActiveTransfer(Transfer *transfer) {
    m_active << transfer;
    emit activeChanged(active());
}

void Transfers::removeActiveTransfer(Transfer *transfer) {
    m_active.removeOne(transfer);
    emit activeChanged(active());
}

void Transfers::onTransferStatusChanged() {
    if (Transfer *transfer = qobject_cast<Transfer*>(sender())) {
        switch (transfer->status()) {
        case Transfer::Paused:
        case Transfer::Failed:
            removeActiveTransfer(transfer);
            break;
        case Transfer::Canceled:
        case Transfer::Completed:
            removeTransfer(transfer);
            storeTransfers();
            break;
        case Transfer::Queued:
            break;
        default:
            return;
        }
                
        if (active() < Settings::instance()->maximumConcurrentTransfers()) {
            m_queueTimer.start();
        }
    }
}

void Transfers::onMaximumConcurrentTransfersChanged() {
    const int max = Settings::instance()->maximumConcurrentTransfers();
    int act = active();
    
    if (act < max) {
        startNextTransfers();
    }
    else if (act > max) {
        for (int priority = Transfer::LowPriority; priority >= Transfer::HighPriority; priority--) {
            for (int i = m_active.size() - 1; i >= 0; i--) {
                if (m_active.at(i)->priority() == priority) {
                    m_active.at(i)->pause();
                    act--;
                
                    if (act == max) {
                        return;
                    }
                }
            }
        }
    }
}
