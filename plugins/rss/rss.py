#!/usr/bin/env python

# Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from datetime import datetime
import getopt
import os
import simplejson as json
import sys
import urllib
import xml.etree.ElementTree as ET

try:
    from configparser import ConfigParser
except ImportError:
    from ConfigParser import ConfigParser
    
class ResourceError(Exception):
    pass
    
def format_datetime(datetime_string):
    try:
        return datetime.strptime(datetime_string, '%a, %d %b %Y %H:%M:%S %Z').strftime('%d %b %Y')
    except:
        return datetime_string

def get_default_urls():
    config = ConfigParser()
    config.read(os.path.expanduser('~/.config/cuteTube2/cuteTube2.conf'))
    
    try:
        return config.get('RSS', 'feeds').replace(' ', '').replace('"', '').split(',')
    except:
        raise ResourceError('{"error": "Unable to load feed urls from config file"}')
        
def get_feed(url):
    try:
        return urllib.urlopen(url).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve feed from %s"}' % url)
    
def list_videos(url):
    if not url:
        raise ResourceError('{"error": "No url specified"}')
        
    root = ET.fromstring(get_feed(url))
    channel = root.find('channel')
    
    try:
        thumb = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}image').attrib['href']
    except:
        thumb = ''
        
    videos = []
    
    for item in channel.findall('item'):
        try:
            video = {}
            id = {}
            stream = {}
            enclosure = item.find('enclosure')
            stream['description'] = enclosure.attrib['type']
            stream['id'] = '0'
            stream['url'] = enclosure.attrib['url']
            id['stream'] = stream
            id['url'] = url
            video['date'] = format_datetime(item.find('pubDate').text)
            video['description'] = item.find('description').text
            video['duration'] = item.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}duration').text
            video['id'] = json.dumps(id)
            video['largeThumbnailUrl'] = thumb
            video['thumbnailUrl'] = thumb
            video['title'] = item.find('title').text
            video['userId'] = url
            video['username'] = item.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}author').text
            videos.append(video)
        except:
            pass
    
    return videos
    
def list_all_videos():
    videos = []
    
    for url in get_default_urls():
        videos += list_videos(url)
    
    return videos
    
def get_playlist(url):
    if not url:
        raise ResourceError('{"error": "No url specified"}')
        
    root = ET.fromstring(get_feed(url))
    channel = root.find('channel')
    
    playlist = {}
    
    try:
        videos = channel.findall('item')
        thumb = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}image').attrib['href']
        playlist['date'] = format_datetime(videos[0].find('pubDate').text)
        playlist['description'] = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}summary').text
        playlist['id'] = url
        playlist['largeThumbnailUrl'] = thumb
        playlist['thumbnailUrl'] = thumb
        playlist['title'] = channel.find('title').text
        playlist['userId'] = url
        playlist['username'] = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}author').text
        playlist['videoCount'] = len(videos)
    except:
        pass
    
    return playlist
    
def list_all_playlists():
    playlists = []
    
    for url in get_default_urls():
        playlists.append(get_playlist(url))
    
    return playlists
    
def get_user(url):
    if not url:
        raise ResourceError('{"error": "No url specified"}')
        
    root = ET.fromstring(get_feed(url))
    channel = root.find('channel')
    
    user = {}
    
    try:
        thumb = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}image').attrib['href']
        user['description'] = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}summary').text
        user['id'] = url
        user['largeThumbnailUrl'] = thumb
        user['thumbnailUrl'] = thumb
        user['userId'] = url
        user['username'] = channel.find('{http://www.itunes.com/dtds/podcast-1.0.dtd}author').text
    except:
        pass
    
    return user

def list_all_users():
    users = []
    
    for url in get_default_urls():
        users.append(get_user(url))
    
    return users

def list_streams(id):
    try:
        d = json.loads(id)
        streams = []
        streams.append(d['stream'])
        return streams
    except:
        raise ResourceError('{"error": "Null/invalid id specified"}')

def get_item(resource, id):
    if resource == 'playlist':
        return get_playlist(id)
    elif resource == 'user':
        return get_user(id)
    else:
        return {}
        
def list_items(resource, id):
    if not resource or resource == 'video':
        if not id:
            return list_all_videos()
        else:
            try:
                return list_videos(json.loads(id)['url'])
            except ValueError:
                return list_videos(id)
    elif resource == 'playlist':
        if not id:
            return list_all_playlists()
        else:
            return [get_playlist(id)]
    elif resource == 'user':
        if not id:
            return list_all_users()
        else:
            return [get_user(id)]
    elif resource == 'stream':
        return list_streams(id)
    else:
        return []
        
def main(method, resource, id):
    if method == 'list':
        print '{"items": %s}' % json.dumps(list_items(resource, id))
    elif method == 'get':
        print json.dumps(get_item(resource, id))
    else:
        raise ResourceError('{"error": "Invalid method specified: %s"}' % method)

if __name__ == '__main__':
    (opts, args) = getopt.getopt(sys.argv[1:], 'm:r:i:')
    
    method = 'list'
    resource = 'video'
    id = ''
    
    for o, a in opts:
        if o == '-m':
            method = a
        elif o == '-r':
            resource = a
        elif o == '-i':
            id = a
    
    try:
        main(method, resource, id)
        exit(0)
    except ResourceError, e:
        print e.args[0]
        exit(1)
