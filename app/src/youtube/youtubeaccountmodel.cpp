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

#include "youtubeaccountmodel.h"
#include "logger.h"
#include "json.h"
#include "youtube.h"
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>

YouTubeAccountModel::YouTubeAccountModel(QObject *parent) :
    QSqlTableModel(parent)
{
    m_roles[UserIdRole] = "userId";
    m_roles[UsernameRole] = "username";
    m_roles[RelatedPlaylistsRole] = "relatedPlaylists";
    m_roles[AccessTokenRole] = "accessToken";
    m_roles[RefreshTokenRole] = "refreshToken";
    m_roles[ScopesRole] = "scopes";
    m_roles[ActiveRole] = "active";
#if QT_VERSION < 0x050000
    setRoleNames(m_roles);
#endif
    setTable("youtubeAccounts");
    setSort(1, Qt::AscendingOrder);
    setEditStrategy(QSqlTableModel::OnRowChange);
    select();
    emit countChanged(rowCount());
}

QString YouTubeAccountModel::errorString() const {
    return lastError().text();
}

#if QT_VERSION >= 0x050000
QHash<int, QByteArray> YouTubeAccountModel::roleNames() const {
    return m_roles;
}
#endif

QVariant YouTubeAccountModel::data(const QModelIndex &idx, int role) const {
    if (role == ActiveRole) {
        return YouTube::userId() == data(idx, UserIdRole);
    }
    
    if (role >= UserIdRole) {
        return QSqlTableModel::data(index(idx.row(), role - UserIdRole));
    }
    
    return QSqlTableModel::data(idx, role);
}

QVariant YouTubeAccountModel::data(int row, const QByteArray &role) const {
    return data(index(row, 0), m_roles.key(role));
}

bool YouTubeAccountModel::addAccount(const QString &userId, const QString &username,
                                     const QVariantMap &relatedPlaylists, const QString &accessToken,
                                     const QString &refreshToken, const QString &scopes) {
    Logger::log(QString("YouTubeAccountModel::addAccount(). User ID: %1, Username: %2, Access token: %3, Refresh token: %4, Scopes: %5").arg(userId).arg(username).arg(accessToken).arg(refreshToken).arg(scopes), Logger::LowVerbosity);
    QSqlField userIdField("userId", QVariant::String);
    userIdField.setValue(userId);
    
    QSqlField usernameField("username", QVariant::String);
    usernameField.setValue(username);
    
    QSqlField relatedPlaylistsField("relatedPlaylists", QVariant::String);
    relatedPlaylistsField.setValue(QtJson::Json::serialize(relatedPlaylists));
    
    QSqlField accessTokenField("accessToken", QVariant::String);
    accessTokenField.setValue(accessToken);
    
    QSqlField refreshTokenField("refreshToken", QVariant::String);
    refreshTokenField.setValue(refreshToken);
    
    QSqlField scopesField("scopes", QVariant::String);
    scopesField.setValue(scopes);

    QSqlRecord record;
    record.append(userIdField);
    record.append(usernameField);
    record.append(relatedPlaylistsField);
    record.append(accessTokenField);
    record.append(refreshTokenField);
    record.append(scopesField);
    
    const int count = rowCount();
    
    for (int i = 0; i < count; i++) {
        if (data(index(i, 0)) == userId) {
            if (setRecord(i, record)) {
                YouTube::setUserId(userId);
                return true;
            }
            
            return false;
        }
    }
    
    if (insertRecord(-1, record)) {
        YouTube::setUserId(userId);
        const int count = rowCount();
        emit dataChanged(index(0, 0), index(count - 1, columnCount() - 1));
        emit countChanged(count);
        return true;
    }
    
    return false;
}

bool YouTubeAccountModel::removeAccount(int row) {
    const QString userId = data(index(row, 0)).toString();
    Logger::log(QString("YouTubeAccountModel::removeAccount(). Row: %1, User ID: %2").arg(row).arg(userId),
                Logger::MediumVerbosity);
    
    if (removeRows(row, 1)) {
        if (userId == YouTube::userId()) {
            if (rowCount() > 0) {
                selectAccount(0);
            }
            else {
                YouTube::setUserId(QString());
            }
        }
        
        emit countChanged(rowCount());
        return true;
    }
    
    return false;
}

bool YouTubeAccountModel::selectAccount(int row) {
    const QString userId = data(index(row, 0)).toString();
    Logger::log(QString("YouTubeAccountModel::selectAccount(). Row: %1, User ID: %2").arg(row).arg(userId),
                Logger::MediumVerbosity);
    
    if (!userId.isEmpty()) {
        YouTube::setUserId(userId);
        emit dataChanged(index(0, 0), index(rowCount() - 1, columnCount() - 1));
        return true;
    }
    
    return false;
}
