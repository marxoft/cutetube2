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

#include "vimeoaccountmodel.h"
#include "vimeo.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

VimeoAccountModel::VimeoAccountModel(QObject *parent) :
    QSqlTableModel(parent)
{
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[AccessTokenRole] = "accessToken";
    m_roles[ScopesRole] = "scopes";
    m_roles[ActiveRole] = "active";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    setTable("vimeoAccounts");
    setSort(1, Qt::AscendingOrder);
    setEditStrategy(QSqlTableModel::OnRowChange);
    select();
    emit countChanged(rowCount());
}

QString VimeoAccountModel::errorString() const {
    return lastError().text();
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> VimeoAccountModel::roleNames() const {
    return m_roles;
}
#endif

QVariant VimeoAccountModel::data(const QModelIndex &idx, int role) const {
    if (role == ActiveRole) {
        return Vimeo::instance()->userId() == data(idx, UserIdRole);
    }
    
    if (role >= UserIdRole) {
        return QSqlTableModel::data(index(idx.row(), role - UserIdRole));
    }
    
    return QSqlTableModel::data(idx, role);
}

QVariant VimeoAccountModel::data(int row, const QByteArray &role) const {
    return data(index(row, 0), m_roles.key(role));
}

bool VimeoAccountModel::addAccount(const QString &userId, const QString &username, const QString &accessToken,
                                   const QString &scopes) {
                                     
    QSqlField userIdField("userId", QVariant::String);
    userIdField.setValue(userId);
    
    QSqlField usernameField("username", QVariant::String);
    usernameField.setValue(username);
        
    QSqlField accessTokenField("accessToken", QVariant::String);
    accessTokenField.setValue(accessToken);
    
    QSqlField scopesField("scopes", QVariant::String);
    scopesField.setValue(scopes);

    QSqlRecord record;
    record.append(userIdField);
    record.append(usernameField);
    record.append(accessTokenField);
    record.append(scopesField);
    
    if (insertRecord(-1, record)) {
        Vimeo::instance()->setUserId(userId);
        const int count = rowCount();
        emit dataChanged(index(0, 0), index(count - 1, columnCount() - 1));
        emit countChanged(count);
        return true;
    }
    
    return false;
}

bool VimeoAccountModel::removeAccount(int row) {
    QString userId = data(index(row, 0)).toString();
    
    if (removeRows(row, 1)) {
        if (userId == Vimeo::instance()->userId()) {
            if (rowCount() > 0) {
                selectAccount(0);
            }
            else {
                Vimeo::instance()->setUserId(QString());
            }
        }
        
        emit countChanged(rowCount());
        return true;
    }
    
    return false;
}

bool VimeoAccountModel::selectAccount(int row) {
    QString userId = data(index(row, 0)).toString();
    
    if (!userId.isEmpty()) {
        Vimeo::instance()->setUserId(userId);
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        return true;
    }
    
    return false;
}
