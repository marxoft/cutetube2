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
import dbus
import getopt
import hashlib
import os
import simplejson as json
import sys

THUMBNAIL_PATH = os.path.expanduser('~/.thumbnails/cropped/')
VIDEO_SEARCH_ORDER = {'date': 'File:Added', 'alphabet': 'Video:Title'}

try:
    from configparser import ConfigParser
except ImportError:
    from ConfigParser import ConfigParser
    
class ResourceError(Exception):
    pass
    
def format_datetime(timestamp):
    try:
        return datetime.fromtimestamp(timestamp).strftime('%d %b %Y')
    except:
        return datetime.now().strftime('%d %b %Y')
        
def format_duration(seconds):
    try:
        if seconds > 3599:
            s = '%H:%M:%S'
        else:
            s = '%M:%S'
        
        return datetime.fromtimestamp(seconds).strftime(s)
    except:
        return ''
        
def format_thumbnail(uri):
    md5 = hashlib.md5()
    md5.update('file://' + uri.replace(' ', '%20'))
    return '%s%s.jpeg' % (THUMBNAIL_PATH, md5.hexdigest())

def get_default_paths():
    config = ConfigParser()
    config.read(os.path.expanduser('~/.config/cuteTube2/cuteTube2.conf'))
    
    try:
        return config.get('Local%20videos', 'paths').replace(', ', ',').replace('"', '').split(',')
    except:
        raise ResourceError('{"error": "Unable to load paths from config file"}')
        
def get_default_order():
    config = ConfigParser()
    config.read(os.path.expanduser('~/.config/cuteTube2/cuteTube2.conf'))
    
    try:
        return config.get('Local%20videos', 'order')
    except:
        return 'Video:Title'
        
def get_path_filter(paths = None):
    if not paths:
        paths = get_default_paths()
        
    if len(paths) == 0:
        return ''
        
    filter = '<rdfq:or>'
    
    for path in paths:
        filter += '<rdfq:startsWith><rdfq:Property name="File:Path" /><rdf:String>%s</rdf:String></rdfq:startsWith>' % path
    
    filter += '</rdfq:or>'
    return filter
    
def list_videos(filter = None, order = None, offset = 0, limit = 20):
    bus = dbus.SessionBus()
    obj = bus.get_object('org.freedesktop.Tracker', '/org/freedesktop/Tracker/Search')
    iface = dbus.Interface(obj, 'org.freedesktop.Tracker.Search')
    
    if not filter:
        filter = '<rdfq:Condition> %s </rdfq:Condition>' % get_path_filter()
        
    if not order:
        order = get_default_order()
    
    items = iface.Query(-1, 'Videos', ['File:Added', 'File:Name', 'Video:Duration', 'Video:Title'], '', [], filter,
    False, [order], order == 'File:Added', offset, limit)
    
    result = {}
    videos = []
    
    id = json.dumps({'filter': filter, 'order': order, 'offset': 0})
        
    for item in items:
        try:
            url = item[0]
            date = item[2]
            name = item[3]
            duration = item[4]
            title = item[5]
            thumbnail = format_thumbnail(url)
            
            video = {}
            video['date'] = format_datetime(int(date))
            video['duration'] = format_duration(int(duration))
            video['id'] = id
            video['largeThumbnailUrl'] = thumbnail
            video['streamUrl'] = 'file://' + url
            video['thumbnailUrl'] = thumbnail
            
            if title != '':
                video['title'] = title
            else:
                video['title'] = name[0 : name.rfind('.')]
            
            video['url'] = url
            videos.append(video)
        except:
            pass    
    
    if len(items) > 0:
        result['next'] = json.dumps({'filter': filter, 'order': order, 'offset': offset + limit})
    
    result['items'] = videos
    return result

def search_videos(query, order):
    try:
        order = VIDEO_SEARCH_ORDER[order]
    except KeyError:
        order = 'Video:Title'
    
    filter = '<rdfq:Condition> <rdfq:or><rdfq:contains><rdfq:Property name="File:Name" /><rdf:String>%s</rdf:String>\
</rdfq:contains><rdfq:contains><rdfq:Property name="Video:Title" /><rdf:String>%s</rdf:String></rdfq:contains>\
</rdfq:or> </rdfq:Condition>' % (query, query)

    return list_videos(filter, order)
        
def list_items(resource, id):
    if not resource or resource == 'video':
        try:
            id = json.loads(id)
            return list_videos(id['filter'], id['order'], id['offset'])
        except:
            return list_videos()
    else:
        return []
        
def search_items(resource, query, order):
    if not resource or resource == 'video':
        return search_videos(query, order)
    else:
        return []
        
def main(method, resource, id, query, order):
    if method == 'list':
        print json.dumps(list_items(resource, id))
    elif method == 'search':
        print json.dumps(search_items(resource, query, order))
    else:
        raise ResourceError('{"error": "Invalid method specified: %s"}' % method)

if __name__ == '__main__':
    (opts, args) = getopt.getopt(sys.argv[1:], 'm:r:i:q:o:')
    
    method = 'list'
    resource = 'video'
    id = ''
    query = ''
    order = ''
    
    for o, a in opts:
        if o == '-m':
            method = a
        elif o == '-r':
            resource = a
        elif o == '-i':
            id = a
        elif o == '-q':
            query = a
        elif o == '-o':
            order = a
    
    try:
        main(method, resource, id, query, order)
        exit(0)
    except ResourceError, e:
        print e.args[0]
        exit(1)
