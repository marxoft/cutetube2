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

#ifndef DAILYMOTIONACCOUNTMODEL_H
#define DAILYMOTIONACCOUNTMODEL_H

#include <QSqlTableModel>

class DailymotionAccountModel : public QSqlTableModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString)
    
public:
    enum Roles {
        UserIdRole = Qt::UserRole + 1,
        UsernameRole,
        AccessTokenRole,
        RefreshTokenRole,
        ScopesRole,
        ActiveRole
    };
    
    explicit DailymotionAccountModel(QObject *parent = 0);
    
    QString errorString() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    Q_INVOKABLE QVariant data(int row, const QByteArray &role) const;

    Q_INVOKABLE bool addAccount(const QString &userId, const QString &username, const QString &accessToken,
                                const QString &refreshToken, const QString &scopes);
    
    Q_INVOKABLE bool removeAccount(int row);
    
    Q_INVOKABLE bool selectAccount(int row);

Q_SIGNALS:
    void countChanged(int c);
    
private:
    QHash<int, QByteArray> m_roles;
};

#endif // DAILYMOTIONACCOUNTMODEL_H
