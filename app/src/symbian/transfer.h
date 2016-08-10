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

#ifndef TRANSFER_H
#define TRANSFER_H

#include <QObject>
#include <QFile>
#include <QPointer>
#include <QStringList>
#include <QUrl>

class QNetworkAccessManager;
class QNetworkReply;
class QProcess;

class Transfer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(qint64 bytesTransferred READ bytesTransferred NOTIFY bytesTransferredChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
    Q_PROPERTY(QString customCommand READ customCommand WRITE setCustomCommand NOTIFY customCommandChanged)
    Q_PROPERTY(bool customCommandOverrideEnabled READ customCommandOverrideEnabled WRITE setCustomCommandOverrideEnabled
               NOTIFY customCommandOverrideEnabledChanged)
    Q_PROPERTY(QString downloadPath READ downloadPath WRITE setDownloadPath NOTIFY downloadPathChanged)
    Q_PROPERTY(bool downloadSubtitles READ downloadSubtitles WRITE setDownloadSubtitles NOTIFY downloadSubtitlesChanged)
    Q_PROPERTY(QString subtitlesLanguage READ subtitlesLanguage WRITE setSubtitlesLanguage
               NOTIFY subtitlesLanguageChanged)
    Q_PROPERTY(QString errorString READ errorString NOTIFY statusChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString id READ id WRITE setId NOTIFY idChanged)
    Q_PROPERTY(Priority priority READ priority WRITE setPriority NOTIFY priorityChanged)
    Q_PROPERTY(QString priorityString READ priorityString NOTIFY priorityChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString progressString READ progressString NOTIFY progressChanged)
    Q_PROPERTY(QString service READ service NOTIFY serviceChanged)
    Q_PROPERTY(qint64 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusString READ statusString NOTIFY statusChanged)
    Q_PROPERTY(QString streamId READ streamId WRITE setStreamId NOTIFY streamIdChanged)
    Q_PROPERTY(QUrl streamUrl READ streamUrl WRITE setStreamUrl NOTIFY streamUrlChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(TransferType transferType READ transferType WRITE setTransferType NOTIFY transferTypeChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY statusChanged)
    Q_PROPERTY(QString videoId READ videoId WRITE setVideoId NOTIFY videoIdChanged)
    
    Q_ENUMS(Priority Status TransferType)
    
public:
    enum Priority {
        HighPriority = 0,
        NormalPriority,
        LowPriority
    };
    
    enum Status {
        Paused = 0,
        Canceled,
        Failed,
        Completed,
        Queued,
        Connecting,
        Downloading,
        Uploading,
        ExecutingCustomCommand,
        Unknown
    };
    
    enum TransferType {
        Download = 0,
        Upload
    };
    
    explicit Transfer(QObject *parent = 0);
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
    qint64 bytesTransferred() const;
    
    QString category() const;
    void setCategory(const QString &c);
    
    QString customCommand() const;
    void setCustomCommand(const QString &c);
    bool customCommandOverrideEnabled() const;
    void setCustomCommandOverrideEnabled(bool enabled);
    
    QString downloadPath() const;
    void setDownloadPath(const QString &path);
    
    bool downloadSubtitles() const;
    void setDownloadSubtitles(bool enabled);
    QString subtitlesLanguage() const;
    void setSubtitlesLanguage(const QString &language);
    
    QString errorString() const;
    
    QString fileName() const;
    void setFileName(const QString &fn);
    
    QString id() const;
    void setId(const QString &i);
        
    Priority priority() const;
    void setPriority(Priority p);
    QString priorityString() const;
    
    int progress() const;
    QString progressString() const;
    
    QString service() const;
    
    qint64 size() const;
    void setSize(qint64 s);
    
    Status status() const;
    QString statusString() const;
    
    QString streamId() const;
    void setStreamId(const QString &si);
    QUrl streamUrl() const;
    void setStreamUrl(const QUrl &url);
    
    QString title() const;
    void setTitle(const QString &title);
    
    TransferType transferType() const;
    void setTransferType(TransferType type);
    
    QUrl url() const;
    
    QString videoId() const;
    void setVideoId(const QString &vi);
    
public Q_SLOTS:
    void queue();
    void start();
    void pause();
    void cancel();
    
protected:
    virtual void listStreams() = 0;
    virtual void listSubtitles() = 0;
    
    void setErrorString(const QString &es);
    
    void setProgress(int p);
    
    void setService(const QString &s);
        
    void setStatus(Status s);
        
    void startDownload(const QUrl &u);
    void followRedirect(const QUrl &u);
        
    void startSubtitlesDownload(const QUrl &u);
    
    bool executeCustomCommands();
    void executeCustomCommand(const QString &command);
    
    void moveDownloadedFiles();
    
private Q_SLOTS:
    void onReplyMetaDataChanged();
    void onReplyReadyRead();
    void onReplyFinished();
    void onSubtitlesReplyFinished();
    void onCustomCommandFinished(int exitCode);
    void onCustomCommandError();
    
Q_SIGNALS:
    void bytesTransferredChanged();
    void categoryChanged();
    void customCommandChanged();
    void customCommandOverrideEnabledChanged();
    void downloadPathChanged();
    void downloadSubtitlesChanged();
    void subtitlesLanguageChanged();
    void fileNameChanged();
    void idChanged();
    void priorityChanged();
    void progressChanged();
    void serviceChanged();
    void sizeChanged();
    void statusChanged();
    void streamIdChanged();
    void streamUrlChanged();
    void titleChanged();
    void transferTypeChanged();
    void videoIdChanged();

private:    
    QPointer<QNetworkAccessManager> m_nam;
    QNetworkReply *m_reply;
    QProcess *m_process;
        
    QFile m_file;
    
    bool m_ownNetworkAccessManager;
    bool m_canceled;
    
    QString m_category;
    
    QString m_customCommand;
    bool m_customCommandOverrideEnabled;
    
    QString m_downloadPath;
    
    bool m_downloadSubtitles;
    QString m_subtitlesLanguage;
    
    QString m_errorString;
    
    QString m_fileName;
    
    QString m_id;
    
    Priority m_priority;
    
    int m_progress;
        
    QString m_service;
    
    qint64 m_size;
    qint64 m_bytesTransferred;
    
    int m_redirects;
    
    Status m_status;
    
    QString m_streamId;
    QUrl m_streamUrl;
    
    QString m_title;
    
    TransferType m_transferType;
    
    QString m_videoId;
    
    QStringList m_commands;
    
    bool m_metadataSet;
};
    
#endif // TRANSFER_H
