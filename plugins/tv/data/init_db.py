#!/usr/bin/env python

import sqlite3
import json

def get_channels():
    with open("channels.txt") as file:
        return json.loads(file.read())["data"]

def get_groups():
    with open("groups.txt") as file:
        return json.loads(file.read())["data"]

def init_db():
    conn = sqlite3.connect("cutetube2-tv.db")
    c = conn.cursor()

    c.execute("CREATE TABLE channels (id INTEGER PRIMARY KEY NOT NULL, title TEXT COLLATE NOCASE, description TEXT, groupId INTEGER NOT NULL, service TEXT)")

    c.execute("CREATE TABLE groups (id INTEGER PRIMARY KEY NOT NULL, title TEXT COLLATE NOCASE, service TEXT)")

    ch = get_channels()
    gr = get_groups()
    
    channels = []
    groups = []
    
    for channel in ch:
        channels.append((channel["id"], channel["title"], channel["description"], channel["group_id"], "filmon"))
    
    for group in gr:
        groups.append((group["id"], group["name"], "filmon"))
    
    c.executemany("INSERT INTO channels VALUES (?, ?, ?, ?, ?)", channels)
    c.executemany("INSERT INTO groups VALUES (?, ?, ?)", groups)
    
    conn.commit()
    conn.close()

if __name__ == "__main__":
    import sys
    sys.exit(init_db())
