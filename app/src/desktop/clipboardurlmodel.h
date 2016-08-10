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

#ifndef CLIPBOARDURLMODEL_H
#define CLIPBOARDURLMODEL_H

#include "stringmodel.h"

class ClipboardUrlModel : public StringModel
{
    Q_OBJECT

    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

public:
    ~ClipboardUrlModel();

    static ClipboardUrlModel* instance();

    bool isEnabled() const;

public Q_SLOTS:
    void setEnabled(bool enabled);

    void restore();
    void save();
    
private Q_SLOTS:
    void onClipboardDataChanged();

Q_SIGNALS:
    void enabledChanged(bool enabled);

private:
    ClipboardUrlModel();

    static ClipboardUrlModel *self;

    bool m_enabled;    
};

#endif // CLIPBOARDURLMODEL_H
