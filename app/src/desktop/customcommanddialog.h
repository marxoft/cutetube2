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

#ifndef CUSTOMCOMMANDDIALOG_H
#define CUSTOMCOMMANDDIALOG_H

#include <QDialog>

class QCheckBox;
class QDialogButtonBox;
class QFormLayout;
class QLineEdit;

class CustomCommandDialog : public QDialog
{
    Q_OBJECT
    
    Q_PROPERTY(QString command READ command WRITE setCommand)
    Q_PROPERTY(bool overrideEnabled READ overrideEnabled WRITE setOverrideEnabled)

public:
    explicit CustomCommandDialog(QWidget *parent = 0);
    
    QString command() const;
    
    bool overrideEnabled() const;

public Q_SLOTS:
    void setCommand(const QString &command);
    
    void setOverrideEnabled(bool enabled);

private:
    QLineEdit *m_commandEdit;
    
    QCheckBox *m_overrideCheckBox;
    
    QDialogButtonBox *m_buttonBox;
    
    QFormLayout *m_layout;
};

#endif // CUSTOMCOMMANDDIALOG_H
