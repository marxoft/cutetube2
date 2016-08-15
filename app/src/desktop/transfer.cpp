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

#include "transfer.h"
#include "definitions.h"
#include "logger.h"
#include "settings.h"
#include "utils.h"
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QProcess>

Transfer::Transfer(QObject *parent) :
    QObject(parent),
    m_nam(0),
    m_reply(0),
    m_process(0),
    m_ownNetworkAccessManager(false),
    m_canceled(false),
    m_category(tr("Default")),
    m_customCommandOverrideEnabled(false),
    m_downloadSubtitles(false),
    m_priority(NormalPriority),
    m_progress(0),
    m_size(0),
    m_bytesTransferred(0),
    m_redirects(0),
    m_status(Paused),
    m_transferType(Download),
    m_metadataSet(false)
{
}

void Transfer::setNetworkAccessManager(QNetworkAccessManager *manager) {
    if ((m_nam) && (m_ownNetworkAccessManager)) {
        delete m_nam;
    }
    
    m_nam = manager;
    m_ownNetworkAccessManager = false;
}

qint64 Transfer::bytesTransferred() const {
    return m_bytesTransferred;
}

QString Transfer::category() const {
    return m_category;
}

void Transfer::setCategory(const QString &c) {
    if (c != category()) {
        m_category = c;
        emit categoryChanged();
    }
}

QString Transfer::customCommand() const {
    return m_customCommand;
}

void Transfer::setCustomCommand(const QString &c) {
    if (c != customCommand()) {
        m_customCommand = c;
        emit customCommandChanged();
    }
}

bool Transfer::customCommandOverrideEnabled() const {
    return m_customCommandOverrideEnabled;
}

void Transfer::setCustomCommandOverrideEnabled(bool enabled) {
    if (enabled != customCommandOverrideEnabled()) {
        m_customCommandOverrideEnabled = enabled;
        emit customCommandOverrideEnabledChanged();
    }
}

QString Transfer::downloadPath() const {
    return m_downloadPath;
}

void Transfer::setDownloadPath(const QString &path) {
    if (path != downloadPath()) {
        m_downloadPath = path.endsWith("/") ? path : path + "/";
        emit downloadPathChanged();
        
        if (!fileName().isEmpty()) {
            m_file.setFileName(downloadPath() + fileName());
            m_bytesTransferred = m_file.size();
            
            if ((m_size > 0) && (m_bytesTransferred > 0)) {
                setProgress(m_bytesTransferred * 100 / m_size);
            }
        }
    }
}

bool Transfer::downloadSubtitles() const {
    return m_downloadSubtitles;
}

void Transfer::setDownloadSubtitles(bool enabled) {
    if (enabled != downloadSubtitles()) {
        m_downloadSubtitles = enabled;
        emit downloadSubtitlesChanged();
    }
}

QString Transfer::subtitlesLanguage() const {
    return m_subtitlesLanguage;
}

void Transfer::setSubtitlesLanguage(const QString &language) {
    if (language != subtitlesLanguage()) {
        m_subtitlesLanguage = language;
        emit subtitlesLanguageChanged();
    }
}

QString Transfer::errorString() const {
    return m_errorString;
}

void Transfer::setErrorString(const QString &es) {
    m_errorString = es;
}

QString Transfer::fileName() const {
    return m_fileName;
}

void Transfer::setFileName(const QString &name) {
    if (name != fileName()) {
        m_fileName = name;

        switch (transferType()) {
        case Transfer::Download:
            m_fileName.replace(ILLEGAL_FILENAME_CHARS_RE, "_");
            break;
        default:
            break;
        }

        emit fileNameChanged();
        
        if (!downloadPath().isEmpty()) {
            m_file.setFileName(downloadPath() + fileName());
            m_bytesTransferred = m_file.size();
            
            if ((m_size > 0) && (m_bytesTransferred > 0)) {
                setProgress(m_bytesTransferred * 100 / m_size);
            }
        }
    }
}

QString Transfer::id() const {
    return m_id;
}

void Transfer::setId(const QString &i) {
    if (i != id()) {
        m_id = i;
        emit idChanged();
    }
}

Transfer::Priority Transfer::priority() const {
    return m_priority;
}

void Transfer::setPriority(Priority p) {
    if (p != priority()) {
        m_priority = p;
        emit priorityChanged();
    }
}

QString Transfer::priorityString() const {
    switch (priority()) {
    case HighPriority:
        return tr("High");
    case NormalPriority:
        return tr("Normal");
    case LowPriority:
        return tr("Low");
    default:
        return QString();
    }
}

int Transfer::progress() const {
    return m_progress;
}

void Transfer::setProgress(int p) {
    if (p != progress()) {
        m_progress = p;
        emit progressChanged();
    }
}

QString Transfer::progressString() const {
    return tr("%1 of %2 (%3%)").arg(Utils::formatBytes(bytesTransferred())).arg(Utils::formatBytes(size()))
                              .arg(progress());
}

QString Transfer::service() const {
    return m_service;
}

void Transfer::setService(const QString &s) {
    if (s != service()) {
        m_service = s;
        emit serviceChanged();
    }
}

qint64 Transfer::size() const {
    return m_size;
}

void Transfer::setSize(qint64 s) {
    if (s != size()) {
        m_size = s;
        emit sizeChanged();
        
        if ((m_size > 0) && (m_bytesTransferred > 0)) {
            setProgress(m_bytesTransferred * 100 / m_size);
        }
    }
}

Transfer::Status Transfer::status() const {
    return m_status;
}

void Transfer::setStatus(Status s) {
    if (s != status()) {
        m_status = s;
        Logger::log(QString("Transfer::setStatus(). ID: %1, Status: %2").arg(id()).arg(statusString()),
                    Logger::LowVerbosity);
        emit statusChanged();
    }
}

QString Transfer::statusString() const {
    switch (status()) {
    case Paused:
        return tr("Paused");
    case Canceled:
        return tr("Canceled");
    case Failed:
        return tr("Failed: %1").arg(errorString());
    case Completed:
        return tr("Completed");
    case Queued:
        return tr("Queued");
    case Connecting:
        return tr("Connecting");
    case Downloading:
        return tr("Downloading");
    case Uploading:
        return tr("Uploading");
    case ExecutingCustomCommand:
        return tr("Executing custom command");
    default:
        return QString();
    }
}

QString Transfer::streamId() const {
    return m_streamId;
}

void Transfer::setStreamId(const QString &si) {
    if (si != streamId()) {
        m_streamId = si;
        emit streamIdChanged();
    }
}

QUrl Transfer::streamUrl() const {
    return m_streamUrl;
}

void Transfer::setStreamUrl(const QUrl &url) {
    if (url != streamUrl()) {
        m_streamUrl = url;
        emit streamUrlChanged();
    }
}

QString Transfer::title() const {
    return m_title;
}

void Transfer::setTitle(const QString &t) {
    if (t != title()) {
        m_title = t;
        emit titleChanged();
    }
}

Transfer::TransferType Transfer::transferType() const {
    return m_transferType;
}

void Transfer::setTransferType(TransferType type) {
    if (type != transferType()) {
        m_transferType = type;
        emit transferTypeChanged();
    }
}

QUrl Transfer::url() const {
    return m_reply ? m_reply->url() : QUrl();
}

QString Transfer::videoId() const {
    return m_videoId;
}

void Transfer::setVideoId(const QString &vi) {
    if (vi != videoId()) {
        m_videoId = vi;
        emit videoIdChanged();
    }
}

void Transfer::queue() {
    switch (status()) {
    case Canceled:
    case Completed:
    case Queued:
    case Connecting:
    case Downloading:
    case Uploading:
    case ExecutingCustomCommand:
        return;
    default:
        break;
    }
    
    setStatus(Queued);
}

void Transfer::start() {
    switch (status()) {
    case Canceled:
    case Completed:
    case Connecting:
    case Downloading:
    case Uploading:
    case ExecutingCustomCommand:
        return;
    default:
        break;
    }
    
    switch (transferType()) {
    case Upload:
        return;
    default:
        break;
    }
    
    setStatus(Connecting);
    
    if (streamUrl().isEmpty()) {
        listStreams();
    }
    else {
        startDownload(streamUrl());
    }
}

void Transfer::pause() {
    switch (status()) {
    case Paused:
    case Canceled:
    case Completed:
    case Connecting:
    case ExecutingCustomCommand:
        return;
    default:
        break;
    }
    
    if ((m_reply) && (m_reply->isRunning())) {
        m_canceled = false;
        m_reply->abort();
    }
    else {
        setStatus(Paused);
    }
}

void Transfer::cancel() {
    switch (status()) {
    case Canceled:
    case Completed:
    case ExecutingCustomCommand:
        return;
    default:
        break;
    }
    
    if ((m_reply) && (m_reply->isRunning())) {
        m_canceled = true;
        m_reply->abort();
    }
    else {
        m_file.remove();
        QDir().rmdir(downloadPath());
        setStatus(Canceled);
    }
}

void Transfer::startDownload(const QUrl &u) {
    Logger::log("Transfer::startDownload(). URL: " + u.toString(), Logger::LowVerbosity);
    QDir().mkpath(downloadPath());
    
    if (!m_file.open(m_file.exists() ? QFile::Append : QFile::WriteOnly)) {
        setErrorString(m_file.errorString());
        setStatus(Failed);
        return;
    }
    
    if (!m_nam) {
        m_nam = new QNetworkAccessManager(this);
        m_ownNetworkAccessManager = true;
    }
    
    QNetworkRequest request(u);
    request.setRawHeader("User-Agent", USER_AGENT);
    
    if (m_bytesTransferred > 0) {
        request.setRawHeader("Range", "bytes=" + QByteArray::number(m_bytesTransferred) + "-");
    }
    
    setStatus(Downloading);
    
    m_redirects = 0;
    m_reply = m_nam->get(request);
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(onReplyMetaDataChanged()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void Transfer::followRedirect(const QUrl &u) {
    Logger::log("Transfer::followRedirect(). URL: " + u.toString(), Logger::LowVerbosity);
    QDir().mkpath(downloadPath());
    
    if (!m_file.open(m_file.exists() ? QFile::Append : QFile::WriteOnly)) {
        setErrorString(m_file.errorString());
        setStatus(Failed);
        return;
    }
    
    m_redirects++;

    if (!m_nam) {
        m_nam = new QNetworkAccessManager(this);
        m_ownNetworkAccessManager = true;
    }
    
    QNetworkRequest request(u);
    request.setRawHeader("User-Agent", USER_AGENT);
    
    if (m_bytesTransferred > 0) {
        request.setRawHeader("Range", "bytes=" + QByteArray::number(m_bytesTransferred) + "-");
    }
    
    m_reply = m_nam->get(request);
    connect(m_reply, SIGNAL(metaDataChanged()), this, SLOT(onReplyMetaDataChanged()));
    connect(m_reply, SIGNAL(readyRead()), this, SLOT(onReplyReadyRead()));
    connect(m_reply, SIGNAL(finished()), this, SLOT(onReplyFinished()));
}

void Transfer::startSubtitlesDownload(const QUrl &u) {
    Logger::log("Transfer::startSubtitlesDownload(). URL: " + u.toString(), Logger::LowVerbosity);
    
    if (!m_nam) {
        m_nam = new QNetworkAccessManager(this);
        m_ownNetworkAccessManager = true;
    }
    
    QNetworkRequest request(u);
    request.setRawHeader("User-Agent", USER_AGENT);
    m_reply = m_nam->get(request);
    connect(m_reply, SIGNAL(finished()), this, SLOT(onSubtitlesReplyFinished()));
}

bool Transfer::executeCustomCommands() {
    Logger::log("Transfer::executeCustomCommands()", Logger::LowVerbosity);
    m_commands.clear();
    QString command = customCommand();
    const QString defaultCommand = Settings::customTransferCommand();
    const bool defaultEnabled = (!defaultCommand.isEmpty()) && (Settings::customTransferCommandEnabled());

    if (!command.isEmpty()) {
        const QString workingDirectory = downloadPath();
        command.replace("%f", fileName());
        m_commands << Command(workingDirectory, command);
        Logger::log(QString("Transfer::executeCustomCommands(): Adding custom command: Working directory: %1, Command: %2")
                           .arg(workingDirectory).arg(command), Logger::LowVerbosity);
    }
    
    if ((defaultEnabled) && ((command.isEmpty()) || (!customCommandOverrideEnabled()))) {
        const QString workingDirectory = downloadPath();
        command = defaultCommand;
        command.replace("%f", fileName());
        m_commands << Command(workingDirectory, command);
        Logger::log(QString("Transfer::executeCustomCommands(): Adding custom command: Working directory: %1, Command: %2")
                           .arg(workingDirectory).arg(command), Logger::LowVerbosity);
    }
    
    if (!m_commands.isEmpty()) {
        setStatus(ExecutingCustomCommand);
        executeCustomCommand(m_commands.takeFirst());
        return true;
    }
    
    return false;
}

void Transfer::executeCustomCommand(const Command &command) {
    if (!m_process) {
        m_process = new QProcess(this);
        connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(onCustomCommandFinished(int)));
        connect(m_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onCustomCommandError()));
    }

    Logger::log(QString("Transfer::executeCustomCommand(): Working directory: %1, Command: %2")
                       .arg(command.workingDirectory).arg(command.command), Logger::LowVerbosity);
    
    if (QDir(command.workingDirectory).exists()) {
        m_process->setWorkingDirectory(command.workingDirectory);
    }
    
    m_process->start(command.command);
}

void Transfer::moveDownloadedFiles() {
    Logger::log("Transfer::moveDownloadedFiles()", Logger::LowVerbosity);
    QDir destDir(Settings::instance()->downloadPath(category()));
    
    if (!destDir.mkpath(destDir.path())) {
        setErrorString(tr("Cannot make download path %1").arg(destDir.path()));
        setStatus(Failed);
        return;
    }
    
    QDir downDir(downloadPath());
    
    foreach (const QString &oldFileName, downDir.entryList(QDir::Files)) {
        int i = 0;
        QString newFileName = QString("%1/%2").arg(destDir.path()).arg(oldFileName);

        while ((destDir.exists(newFileName)) && (i < 100)) {
            i++;
            newFileName = (i == 1 ? QString("%1(%2)%3").arg(newFileName.left(newFileName.lastIndexOf('.')))
                                                       .arg(i).arg(newFileName.mid(newFileName.lastIndexOf('.')))
                                  : QString("%1(%2)%3").arg(newFileName.left(newFileName.lastIndexOf('(')))
                                                       .arg(i).arg(newFileName.mid(newFileName.lastIndexOf('.'))));
        }
        
        if (!destDir.rename(downDir.absoluteFilePath(oldFileName), newFileName)) {
            setErrorString(tr("Cannot rename downloaded file to %1").arg(newFileName));
            setStatus(Failed);
            return;
        }
    }
        
    downDir.rmdir(downDir.path());
    setErrorString(QString());
    setStatus(Completed);
}

void Transfer::onReplyMetaDataChanged() {
    if ((m_metadataSet) || (m_reply->error() != QNetworkReply::NoError) || (!m_reply->rawHeader("Location").isEmpty())) {
        return;
    }

    qint64 bytes = m_reply->header(QNetworkRequest::ContentLengthHeader).toLongLong();
    
    if (bytes <= 0) {
        bytes = m_reply->rawHeader("Content-Length").toLongLong();
    }
    
    if (bytes > 0) {
        setSize(bytes + bytesTransferred());
    }
    
    m_metadataSet = true;
}

void Transfer::onReplyReadyRead() {
    if (!m_metadataSet) {
        return;
    }

    const qint64 bytes = m_reply->bytesAvailable();

    if (bytes < DOWNLOAD_BUFFER_SIZE) {
        return;
    }

    if (m_file.write(m_reply->read(bytes)) == -1) {
        m_reply->deleteLater();
	m_reply = 0;
        setErrorString(tr("Cannot write to file - %1").arg(m_file.errorString()));
        setStatus(Failed);
        return;
    }
    
    m_bytesTransferred += bytes;
    emit bytesTransferredChanged();
    
    if (m_size > 0) {
        setProgress(m_bytesTransferred * 100 / m_size);
    }
}

void Transfer::onReplyFinished() {
    const QString redirect = QString::fromUtf8(m_reply->rawHeader("Location"));

    if (!redirect.isEmpty()) {
	m_file.close();
        m_reply->deleteLater();
        m_reply = 0;
        
        if (m_redirects < MAX_REDIRECTS) {
            followRedirect(redirect);
        }
        else {
            setErrorString(tr("Maximum redirects reached"));
            setStatus(Failed);
        }
        
        return;
    }

    const QNetworkReply::NetworkError error = m_reply->error();
    const QString errorString = m_reply->errorString();

    if ((m_reply->isOpen()) && (error == QNetworkReply::NoError) && (m_file.isOpen())) {
        const qint64 bytes = m_reply->bytesAvailable();
        
        if ((bytes > 0) && (m_metadataSet)) {
            m_file.write(m_reply->read(bytes));
            m_bytesTransferred += bytes;
            
            if (m_size > 0) {
                setProgress(m_bytesTransferred * 100 / m_size);
            }
        }
    }

    m_file.close();
    m_reply->deleteLater();
    m_reply = 0;
        
    switch (error) {
    case QNetworkReply::NoError:
        break;
    case QNetworkReply::OperationCanceledError:
        setErrorString(QString());
        
        if (m_canceled) {
            m_file.remove();
            QDir().rmdir(downloadPath());
            setStatus(Canceled);
        }
        else {
            setStatus(Paused);
        }
        
        return;
    default:
        setErrorString(errorString);
        setStatus(Failed);
        return;
    }
    
    if (downloadSubtitles()) {
        listSubtitles();
    }
    else if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}

void Transfer::onSubtitlesReplyFinished() {
    switch (m_reply->error()) {
    case QNetworkReply::NoError:
    case QNetworkReply::OperationCanceledError:
        break;
    default:
        if (!executeCustomCommands()) {
            moveDownloadedFiles();
        }
        
        return;
    }
    
    QFile file(m_file.fileName().left(m_file.fileName().lastIndexOf('.')) + ".srt");
    
    if (file.open(QFile::WriteOnly)) {
        file.write(m_reply->readAll());
        file.close();
    }
    
    m_reply->deleteLater();
    m_reply = 0;
    
    if (!executeCustomCommands()) {
        moveDownloadedFiles();
    }
}

void Transfer::onCustomCommandFinished(int exitCode) {
    if (exitCode != 0) {
        Logger::log("Transfer::onCustomCommandFinished(): Error: " + m_process->readAllStandardError());
    }
    
    if (!m_commands.isEmpty()) {
        executeCustomCommand(m_commands.takeFirst());
    }
    else {
        moveDownloadedFiles();
    }
}

void Transfer::onCustomCommandError() {
    Logger::log("Transfer::onCustomCommandError(): " + m_process->errorString());

    if (!m_commands.isEmpty()) {
        executeCustomCommand(m_commands.takeFirst());
    }
    else {
        moveDownloadedFiles();
    }
}
