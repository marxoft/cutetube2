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

#ifndef TRANSFER_H
#define TRANSFER_H

#include <QString>
#include <QUrl>
#include <QFile>
#include <QVariantList>
#include <qplatformdefs.h>

#ifdef MEEGO_EDITION_HARMATTAN
namespace TransferUI {
    class Client;
    class Transfer;
}
#endif

namespace QDailymotion {
    class ResourcesRequest;
    class StreamsRequest;
}

namespace QVimeo {
    class ResourcesRequest;
    class StreamsRequest;
}

namespace QYouTube {
    class StreamsRequest;
    class SubtitlesRequest;
}

class AudioConverter;
class ResourcesRequest;
class QNetworkAccessManager;
class QNetworkReply;

class Transfer : public QObject
{
    Q_OBJECT
    
    Q_PROPERTY(qint64 bytesTransferred READ bytesTransferred NOTIFY progressChanged)
    Q_PROPERTY(bool canConvertToAudio READ canConvertToAudio NOTIFY canConvertToAudioChanged)
    Q_PROPERTY(bool convertToAudio READ convertToAudio WRITE setConvertToAudio NOTIFY convertToAudioChanged)
    Q_PROPERTY(QString category READ category WRITE setCategory NOTIFY categoryChanged)
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
    Q_PROPERTY(QString resourceId READ resourceId WRITE setResourceId NOTIFY resourceIdChanged)
    Q_PROPERTY(QString service READ service WRITE setService NOTIFY serviceChanged)
    Q_PROPERTY(qint64 size READ size WRITE setSize NOTIFY sizeChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
    Q_PROPERTY(QString statusString READ statusString NOTIFY statusChanged)
    Q_PROPERTY(QString streamId READ streamId WRITE setStreamId NOTIFY streamIdChanged)
    Q_PROPERTY(QString title READ title WRITE setTitle NOTIFY titleChanged)
    Q_PROPERTY(TransferType transferType READ transferType WRITE setTransferType NOTIFY transferTypeChanged)
    Q_PROPERTY(QUrl url READ url NOTIFY statusChanged)
    
    Q_ENUMS(Priority Status TransferType)
    
public:
    enum Priority {
        HighPriority,
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
        Converting,
        Unknown
    };
    
    enum TransferType {
        Download,
        Upload
    };
    
    explicit Transfer(QObject *parent = 0);
    ~Transfer();
    
    void setNetworkAccessManager(QNetworkAccessManager *manager);
    
    qint64 bytesTransferred() const;
    
    bool canConvertToAudio() const;
    bool convertToAudio() const;
    void setConvertToAudio(bool enabled);
    
    QString category() const;
    void setCategory(const QString &c);
    
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
    
    QString resourceId() const;
    void setResourceId(const QString &ri);
    
    QString service() const;
    void setService(const QString &s);
    
    qint64 size() const;
    void setSize(qint64 s);
    
    Status status() const;
    QString statusString() const;
    
    QString streamId() const;
    void setStreamId(const QString &si);
    
    QString title() const;
    void setTitle(const QString &title);
    
    TransferType transferType() const;
    void setTransferType(TransferType type);
    
    QUrl url() const;
    
public Q_SLOTS:
    void queue();
    void start();
    void pause();
    void cancel();
    
private:
    void setErrorString(const QString &es);
    
    void setProgress(int p);
        
    void setStatus(Status s);
    
    void listStreams();
    QUrl getStreamUrl(const QVariantList &list) const;
    
    void startDownload(const QUrl &u);
    void followRedirect(const QUrl &u);
    
    void listSubtitles();
    QUrl getSubtitlesUrl(const QVariantList &list) const;
    
    void startSubtitlesDownload(const QUrl &u);
    
    void startAudioConversion();
    
    void moveDownloadedFiles();
    
private Q_SLOTS:
    void onReplyMetaDataChanged();
    void onReplyReadyRead();
    void onReplyFinished();
    void onStreamsRequestFinished();
    void onSubtitlesRequestFinished();
    void onSubtitlesReplyFinished();
    void onAudioConversionFinished();
    void onAudioConversionError();
    
Q_SIGNALS:
    void canConvertToAudioChanged();
    void convertToAudioChanged();
    void categoryChanged();
    void downloadPathChanged();
    void downloadSubtitlesChanged();
    void subtitlesLanguageChanged();
    void fileNameChanged();
    void idChanged();
    void priorityChanged();
    void progressChanged();
    void resourceIdChanged();
    void serviceChanged();
    void sizeChanged();
    void statusChanged();
    void streamIdChanged();
    void titleChanged();
    void transferTypeChanged();

private:
#ifdef MEEGO_EDITION_HARMATTAN
    static TransferUI::Client *tuiClient;
    TransferUI::Transfer *m_tuiTransfer;
#endif

    QDailymotion::StreamsRequest *m_dailymotionStreamsRequest;
    QVimeo::StreamsRequest *m_vimeoStreamsRequest;
    QYouTube::StreamsRequest *m_youtubeStreamsRequest;
    
    QDailymotion::ResourcesRequest *m_dailymotionSubtitlesRequest;
    QVimeo::ResourcesRequest *m_vimeoSubtitlesRequest;
    QYouTube::SubtitlesRequest *m_youtubeSubtitlesRequest;
    
    ResourcesRequest *m_streamsRequest;
    
    AudioConverter *m_audioConverter;
    
    QNetworkAccessManager *m_nam;
    QNetworkReply *m_reply;
        
    QFile m_file;
    
    bool m_ownNetworkAccessManager;
    bool m_canceled;
    
    mutable bool m_canConvertToAudio;
    mutable bool m_canConvertToAudioChecked;
    mutable bool m_convertToAudio;
    
    QString m_category;
    
    QString m_downloadPath;
    
    bool m_downloadSubtitles;
    QString m_subtitlesLanguage;
    
    QString m_errorString;
    
    QString m_fileName;
    
    QString m_id;
    
    Priority m_priority;
    
    int m_progress;
    
    QString m_resourceId;
    
    QString m_service;
    
    qint64 m_size;
    qint64 m_bytesTransferred;
    
    int m_redirects;
    
    Status m_status;
    
    QString m_streamId;
    
    QString m_title;
    
    TransferType m_transferType;
};
    
#endif // TRANSFER_H
