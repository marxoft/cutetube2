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

#ifndef TRANSFERS_H
#define TRANSFERS_H

#include "transfer.h"
#include <QTimer>

class QNetworkAccessManager;

class Transfers : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(int active READ active NOTIFY activeChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    
public:
    explicit Transfers(QObject *parent = 0);
    ~Transfers();
    
    static Transfers* instance();
        
    int active() const;
    int count() const;
    
    Q_INVOKABLE void addDownloadTransfer(const QString &service, const QString &resourceId, const QString &streamId,
                                         const QUrl &streamUrl, const QString &title, const QString &category,
                                         const QString &subtitlesLanguage = QString(), bool convertToAudio = false);
    
    Q_INVOKABLE Transfer* get(int i) const;
    Q_INVOKABLE Transfer* get(const QString &id) const;
    
public Q_SLOTS:
    bool start();
    bool pause();
    bool start(const QString &id);
    bool pause(const QString &id);
    bool cancel(const QString &id);
    
    void storeTransfers();
    void restoreTransfers();
    
private:
    void getNextTransfers();
    
    void removeTransfer(Transfer *transfer);

    void addActiveTransfer(Transfer *transfer);
    void removeActiveTransfer(Transfer *transfer);

private Q_SLOTS:
    void startNextTransfers();
    
    void onTransferStatusChanged();
    void onMaximumConcurrentTransfersChanged();
    
Q_SIGNALS:
    void activeChanged(int a);
    void countChanged(int c);
    void transferAdded(Transfer *transfer);
    
private:
    static Transfers *self;
        
    QNetworkAccessManager *m_nam;
    
    QTimer m_queueTimer;
    
    QList<Transfer*> m_transfers;
    QList<Transfer*> m_active;
};
    
#endif // TRANSFERS_H
