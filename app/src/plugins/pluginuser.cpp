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

#include "pluginuser.h"
#include "resources.h"

PluginUser::PluginUser(QObject *parent) :
    User(parent),
    m_request(new ResourcesRequest(this))
{
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginUser::PluginUser(const QString &service, const QString &id, QObject *parent) :
    User(parent),
    m_request(new ResourcesRequest(this))
{
    loadUser(service, id);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginUser::PluginUser(const QString &service, const QVariantMap &user, QObject *parent) :
    User(parent),
    m_request(new ResourcesRequest(this))
{
    loadUser(service, user);
    connect(m_request, SIGNAL(finished()), this, SLOT(onRequestFinished()));
}

PluginUser::PluginUser(const PluginUser *user, QObject *parent) :
    User(user, parent),
    m_request(new ResourcesRequest(this))
{
}

QString PluginUser::errorString() const {
    return m_request->errorString();
}

ResourcesRequest::Status PluginUser::status() const {
    return m_request->status();
}

void PluginUser::loadUser(const QString &service, const QString &id) {
    if (status() == ResourcesRequest::Loading) {
        return;
    }
    
    m_request->setService(service);
    m_request->get(Resources::USER, id);

    emit statusChanged(status());
}

void PluginUser::loadUser(const QString &service, const QVariantMap &user) {
    setService(service);
    setDescription(user.value("description").toString());
    setId(user.value("id").toString());
    setLargeThumbnailUrl(user.value("largeThumbnailUrl").toString());
    setThumbnailUrl(user.value("thumbnailUrl").toString());
    setUsername(user.value("username").toString());
}

void PluginUser::loadUser(PluginUser *user) {
    User::loadUser(user);
}

void PluginUser::onRequestFinished() {
    if (m_request->status() == ResourcesRequest::Ready) {
        loadUser(m_request->service(), m_request->result().toMap());
    }
    
    emit statusChanged(status());
}
