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

#ifndef YOUTUBEACCOUNTMODEL_H
#define YOUTUBEACCOUNTMODEL_H

#include <QSqlTableModel>

class YouTubeAccountModel : public QSqlTableModel
{
    Q_OBJECT
    
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
    Q_PROPERTY(QString errorString READ errorString)
    
public:
    enum Roles {
        UserIdRole = Qt::UserRole + 1,
        UsernameRole,
        RelatedPlaylistsRole,
        AccessTokenRole,
        RefreshTokenRole,
        ScopesRole,
        ActiveRole
    };
    
    explicit YouTubeAccountModel(QObject *parent = 0);
    
    QString errorString() const;
    
#if QT_VERSION >= 0x050000
    QHash<int, QByteArray> roleNames() const;
#endif
    QVariant data(const QModelIndex &idx, int role = Qt::DisplayRole) const;
    QVariant data(int row, const QByteArray &role) const;

public Q_SLOTS:
    bool addAccount(const QString &userId, const QString &username, const QVariantMap &relatedPlaylists,
                    const QString &accessToken, const QString &refreshToken, const QString &scopes);
    
    bool removeAccount(int row);
    
    bool selectAccount(int row);

Q_SIGNALS:
    void countChanged(int c);
    
private:
    QHash<int, QByteArray> m_roles;
};

#endif // YOUTUBEACCOUNTMODEL_H
