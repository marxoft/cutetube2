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

#include "dailymotionaccountmodel.h"
#include "dailymotion.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

DailymotionAccountModel::DailymotionAccountModel(QObject *parent) :
    QSqlTableModel(parent)
{
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[AccessTokenRole] = "accessToken";
    m_roles[RefreshTokenRole] = "refreshToken";
    m_roles[ScopesRole] = "scopes";
    m_roles[ActiveRole] = "active";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    setTable("dailymotionAccounts");
    setSort(1, Qt::AscendingOrder);
    setEditStrategy(QSqlTableModel::OnRowChange);
    select();
    emit countChanged(rowCount());
}

QString DailymotionAccountModel::errorString() const {
    return lastError().text();
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> DailymotionAccountModel::roleNames() const {
    return m_roles;
}
#endif

QVariant DailymotionAccountModel::data(const QModelIndex &idx, int role) const {
    if (role == ActiveRole) {
        return Dailymotion::instance()->userId() == data(idx, UserIdRole);
    }
    
    if (role >= UserIdRole) {
        return QSqlTableModel::data(index(idx.row(), role - UserIdRole));
    }
    
    return QSqlTableModel::data(idx, role);
}

QVariant DailymotionAccountModel::data(int row, const QByteArray &role) const {
    return data(index(row, 0), m_roles.key(role));
}

bool DailymotionAccountModel::addAccount(const QString &userId, const QString &username, const QString &accessToken,
                                         const QString &refreshToken, const QString &scopes) {
                                     
    QSqlField userIdField("userId", QVariant::String);
    userIdField.setValue(userId);
    
    QSqlField usernameField("username", QVariant::String);
    usernameField.setValue(username);
        
    QSqlField accessTokenField("accessToken", QVariant::String);
    accessTokenField.setValue(accessToken);
    
    QSqlField refreshTokenField("refreshToken", QVariant::String);
    refreshTokenField.setValue(refreshToken);
    
    QSqlField scopesField("scopes", QVariant::String);
    scopesField.setValue(scopes);

    QSqlRecord record;
    record.append(userIdField);
    record.append(usernameField);
    record.append(accessTokenField);
    record.append(refreshTokenField);
    record.append(scopesField);
    
    if (insertRecord(-1, record)) {
        Dailymotion::instance()->setUserId(userId);
        const int count = rowCount();
        emit dataChanged(index(0, 0), index(count - 1, columnCount() - 1));
        emit countChanged(count);
        return true;
    }
    
    return false;
}

bool DailymotionAccountModel::removeAccount(int row) {
    QString userId = data(index(row, 0)).toString();
    
    if (removeRows(row, 1)) {
        if (userId == Dailymotion::instance()->userId()) {
            if (rowCount() > 0) {
                selectAccount(0);
            }
            else {
                Dailymotion::instance()->setUserId(QString());
            }
        }
        
        emit countChanged(rowCount());
        return true;
    }
    
    return false;
}

bool DailymotionAccountModel::selectAccount(int row) {
    QString userId = data(index(row, 0)).toString();
    
    if (!userId.isEmpty()) {
        Dailymotion::instance()->setUserId(userId);
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        return true;
    }
    
    return false;
}
