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

#include "audioconverter.h"
#include <QFile>

static const QString CONVERT_COMMAND(QFile::exists("/usr/bin/avconv") ? "avconv" : "ffmpeg");

AudioConverter::AudioConverter(QObject *parent) :
    QObject(parent)
{
    connect(&m_process, SIGNAL(finished(int,QProcess::ExitStatus)),
            this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));
}

QString AudioConverter::fileName() const {
    return m_fileName;
}

void AudioConverter::setFileName(const QString &fileName) {
    m_fileName = fileName;
}

QString AudioConverter::outputDirectory() const {
    return m_outputDirectory;
}

void AudioConverter::setOutputDirectory(const QString &directory) {
    m_outputDirectory = directory.endsWith('/') ? directory : directory + '/';
}

QString AudioConverter::errorString() const {
    return m_errorString;
}

void AudioConverter::setErrorString(const QString &errorString) {
    m_errorString = errorString;
}

void AudioConverter::start() {
    if ((fileName().isEmpty()) || (outputDirectory().isEmpty())) {
        setErrorString(tr("No input filename and/or output directory specified"));
        emit error();
        return;
    }

    if (m_process.state() != QProcess::Running) {
        m_process.start(QString("%1 -i \"%2\" -acodec copy -y -vn \"%3\"")
                               .arg(CONVERT_COMMAND)
                               .arg(fileName()).arg(outputDirectory() + fileName().section('/', -1)
                               .section('.', 0, -2) + ".m4a"));
    }
}

void AudioConverter::start(const QString &fileName, const QString &outputDirectory) {
    setFileName(fileName);
    setOutputDirectory(outputDirectory);
    start();
}

void AudioConverter::onProcessFinished(int exitCode, QProcess::ExitStatus status) {
    if ((exitCode == 0) && (status == QProcess::NormalExit)) {
        emit finished();
    }
    else {
        setErrorString(m_process.errorString());
        emit error();
    }
}
