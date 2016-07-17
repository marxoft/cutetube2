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

#ifndef DATABASE_H
#define DATABASE_H

#include "definitions.h"
#include "logger.h"
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>

inline void initDatabase() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    QDir().mkpath(APP_CONFIG_PATH);
    db.setDatabaseName(APP_CONFIG_PATH + "cuteTube2.db");
    
    if (!db.isOpen()) {
        db.open();
    }
    
    QSqlQuery query = db.exec("CREATE TABLE IF NOT EXISTS dailymotionAccounts (userId TEXT UNIQUE, username TEXT, \
    accessToken TEXT, refreshToken TEXT, scopes TEXT)");
    
    if (query.lastError().isValid()) {
        Logger::log("initDatabase: database error: " + query.lastError().text());
    }
    
    query = db.exec("CREATE TABLE IF NOT EXISTS vimeoAccounts (userId TEXT UNIQUE, username TEXT, accessToken TEXT, \
    scopes TEXT)");
    
    if (query.lastError().isValid()) {
        Logger::log("initDatabase: database error: " + query.lastError().text());
    }
    
    query = db.exec("CREATE TABLE IF NOT EXISTS youtubeAccounts (userId TEXT UNIQUE, username TEXT, \
    relatedPlaylists TEXT, accessToken TEXT, refreshToken TEXT, scopes TEXT)");
    
    if (query.lastError().isValid()) {
        Logger::log("initDatabase: database error: " + query.lastError().text());
    }
}

inline QSqlDatabase getDatabase() {
    QSqlDatabase db = QSqlDatabase::database();

    if (!db.isOpen()) {
        db.open();
    }
    
    return db;
}

#endif // DATABASE_H
