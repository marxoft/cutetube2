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

#ifndef AUDIOCONVERTER_H
#define AUDIOCONVERTER_H

#include <QObject>
#include <QProcess>

class AudioConverter : public QObject
{
    Q_OBJECT

public:
    explicit AudioConverter(QObject *parent = 0);

    QString fileName() const;
    void setFileName(const QString &fileName);

    QString outputDirectory() const;
    void setOutputDirectory(const QString &directory);

    QString errorString() const;
    
public slots:
    void start();
    void start(const QString &fileName, const QString &outputDirectory);

private:
    void setErrorString(const QString &errorString);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus status);

signals:
    void finished();
    void error();

private:
    QProcess m_process;
    
    QString m_fileName;
    QString m_outputDirectory;
    QString m_errorString;
};

#endif // AUDIOCONVERTER_H
