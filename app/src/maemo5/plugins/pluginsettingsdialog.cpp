/*
 * Copyright (C) 2014 Stuart Howarth <showarth@marxoft.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 3, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "pluginsettingsdialog.h"
#include "pluginsettingsselector.h"
#include "pluginsettingscheckbox.h"
#include "pluginsettingsspinbox.h"
#include "pluginsettingslineedit.h"
#include "selectionmodel.h"
#include <QFile>
#include <QLabel>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QXmlStreamReader>

PluginSettingsDialog::PluginSettingsDialog(const QString &name, const QString &fileName, QWidget *parent) :
    Dialog(parent)
{
    setWindowTitle(name);
    QHBoxLayout *hbox = new QHBoxLayout(this);
    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Vertical, this);
    QScrollArea *scrollArea = new QScrollArea(this);
    QWidget *scrollWidget = new QWidget(scrollArea);
    QVBoxLayout *vbox = new QVBoxLayout(scrollWidget);
    vbox->setContentsMargins(0, 0, 0, 0);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(scrollWidget);

    QFile file(fileName);
    
    if (file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader reader;
        reader.setDevice(&file);
        reader.readNextStartElement();

        while (!reader.atEnd()) {
            if (!reader.attributes().isEmpty()) {
                if (reader.name() == "group") {
                    vbox->addWidget(new QLabel(reader.attributes().value("title").toString(), this));
                }
                else if (reader.name() == "list") {
                    PluginSettingsSelector *selector = new PluginSettingsSelector(reader.attributes().value("title")
                                                                                                     .toString(), this);
                    selector->setKey(QString("%1/%2").arg(windowTitle()).arg(reader.attributes().value("key").toString()));
                    selector->setDefaultValue(reader.attributes().value("default").toString());
                    reader.readNextStartElement();

                    while (reader.name() == "element") {
                        if (!reader.attributes().isEmpty()) {
                            selector->model()->append(reader.attributes().value("name").toString(),
                                                      reader.attributes().value("value").toString());
                        }

                        reader.readNextStartElement();
                    }

                    selector->load();
                    vbox->addWidget(selector);
                }
                else if (reader.name() == "boolean") {
                    PluginSettingsCheckbox *checkbox = new PluginSettingsCheckbox(this);
                    checkbox->setText(reader.attributes().value("title").toString());
                    checkbox->setKey(QString("%1/%2").arg(windowTitle()).arg(reader.attributes().value("key").toString()));
                    checkbox->setDefaultValue(reader.attributes().value("default").toString());
                    checkbox->load();
                    vbox->addWidget(checkbox);
                }
                else if (reader.name() == "integer") {
                    PluginSettingsSpinbox *spinbox = new PluginSettingsSpinbox(this);
                    spinbox->setKey(QString("%1/%2").arg(windowTitle()).arg(reader.attributes().value("key").toString()));
                    spinbox->setDefaultValue(reader.attributes().value("default").toString());
                    spinbox->setMinimum(reader.attributes().value("min").toString().toInt());
                    spinbox->setMaximum(reader.attributes().value("max").toString().toInt());
                    spinbox->setSingleStep(reader.attributes().value("step").toString().toInt());
                    spinbox->load();
                    vbox->addWidget(new QLabel(reader.attributes().value("title").toString(), this));
                    vbox->addWidget(spinbox);
                }
                else if (reader.name() == "text") {
                    PluginSettingsLineEdit *lineEdit = new PluginSettingsLineEdit(this);
                    lineEdit->setKey(QString("%1/%2").arg(windowTitle()).arg(reader.attributes().value("key").toString()));
                    lineEdit->setDefaultValue(reader.attributes().value("default").toString());
                    lineEdit->load();
                    vbox->addWidget(new QLabel(reader.attributes().value("title").toString(), this));
                    vbox->addWidget(lineEdit);
                }
            }

            reader.readNextStartElement();
        }

        file.close();

        hbox->addWidget(scrollArea, 0, Qt::AlignBottom);
        hbox->addWidget(buttonBox, 0, Qt::AlignBottom);
    }
    else {
        QMessageBox::critical(this, tr("Error"), file.errorString());
    }

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
}
