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

#ifndef VIMEOACCOUNTMODEL_H
#define VIMEOACCOUNTMODEL_H

#include <QSqlTableModel>

class VimeoAccountModel : public QSqlTableModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString)
    
public:
    enum Roles {
        UserIdRole = Qt::UserRole + 1,
        UsernameRole,
        AccessTokenRole,
        ScopesRole,
        ActiveRole
    };
    
    explicit VimeoAccountModel(QObject *parent = 0);
    
    QString errorString() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;

    Q_INVOKABLE bool addAccount(const QString &userId, const QString &username, const QString &accessToken,
                                const QString &scopes);
    
    Q_INVOKABLE bool removeAccount(int row);
    
    Q_INVOKABLE bool selectAccount(int row);

Q_SIGNALS:
    void countChanged(int c);
    
private:
    QHash<int, QByteArray> m_roles;
};

#endif // VIMEOACCOUNTMODEL_H
