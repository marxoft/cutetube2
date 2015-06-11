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

from BeautifulSoup import BeautifulSoup
from datetime import datetime
import getopt
import re
import simplejson as json
import sys
import urllib

class ResourceError(Exception):
    pass

def format_date(date_string, date_format):    
    try:
        return datetime.strptime(date_string, date_format).strftime('%d %b %Y')
    except:
        return date_string

def format_duration(secs):
    try:
        if secs >= 3600:
            return datetime.fromtimestamp(secs).strftime('%H:%M:%S')
        
        return datetime.fromtimestamp(secs).strftime('%M:%S')
    except:
        return '--:--'

def get_page(url):
    try:
        return urllib.urlopen(url).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve page from %s"}' % url)

def list_videos(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    page = BeautifulSoup(get_page(url))
    result = {}
    videos = []
    
    for att in [{'id': 'Catalog1'}, {'id': 'Catalog2'}, {'data-catalog-id': '1'}, {'data-catalog-id': '2'}]:
        catalog = page.find('ul', attrs=att)
        
        if catalog is not None:
            articles = catalog.findAll('article')
            
            if len(articles) > 0:
                for article in articles:
                    try:
                        info = article.find('h2', attrs={'class': 'ItemInfo'})
                        thumb = article.find('div', attrs={'class': 'ItemThumb'})
                        title = article.find('h1', attrs={'class': 'ItemTitle'}).find('a')
                        user = info.find('a')
        
                        id = title['href']
                        thumbnail_url = thumb.find('img')['src']
                        user_id = user['href']
        
                        if (id[0] == '/'):
                            id = 'http://www.metacafe.com' + id
        
                        if (thumbnail_url[0] == '/'):
                            thumbnail_url = 'http://www.metacafe.com' + thumbnail_url
        
                        if (user_id[0] == '/'):
                            user_id = 'http://www.metacafe.com' + user_id
        
                        video = {}
                        video['date'] = format_date(re.search(r'\d+/\d+/\d+', str(info)).group(0), '%m/%d/%y')
                        video['duration'] = thumb.find('span').string
                        video['id'] = id
                        video['largeThumbnailUrl'] = thumbnail_url.replace('/videos/', '/flash_player/')
                        video['thumbnailUrl'] = thumbnail_url
                        video['title'] = title.string
                        video['url'] = id     
                        video['userId'] = user_id
                        video['username'] = user.string
                        videos.append(video)
                    except:
                        pass
                break
    
    result['items'] = videos
    
    try:
        result['next'] = 'http://www.metacafe.com' + page.find('li', attrs={'id': 'Next'}).find('a')['href']
    except:
        try:
            result['next'] = page.find('a', attrs={'class': 'LoadMore'})['href']
        except:
            pass
    
    return result

def search_videos(query, order):
    return list_videos('http://www.metacafe.com/videos_about/%s/%s' % (query.replace(' ', '_'), order))

def get_video(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url))
        user = page.find('div', attrs={'id': 'ChannelAvatar'}).find('a')
        user_id = user['href']
        thumbnail_url = page.find('meta', attrs={'property': 'og:image'})['content']
    
        if user_id[0] == '/':
            user_id = 'http://www.metacafe.com' + user_id

        if thumbnail_url[0] == '/':
            thumbnail_url = 'http://www.metacafe.com' + thumbnail_url

        video = {}
        video['date'] = format_date(re.sub(r'\s', '', re.search(r'(?<=</small>)[^<]+', str(page.find('h2', attrs={'id': 'UploadInfo'}))).group(0)), '%B%d,%Y')
        video['description'] = page.find('meta', attrs={'property': 'og:description'})['content']
        video['duration'] = format_duration(int(page.find('meta', attrs={'property': 'video:duration'})['content']))
        video['id'] = url
        video['largeThumbnailUrl'] = thumbnail_url
        video['thumbnailUrl'] = thumbnail_url.replace('/flash_player/', '/videos/')
        video['title'] = page.find('meta', attrs={'property': 'og:title'})['content']
        video['url'] = url
        video['userId'] = user_id
        video['username'] = user.find('span').string
        return video
    except:
        raise ResourceError('{"error": "Video not found"}')

def list_streams(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    result = {}
    streams = []
    
    try:
        page = get_page(url)
        media_data = json.loads(urllib.unquote(re.search(r'(?<=mediaData=)[^&]+', page).group(0)))
        
        for media_format in media_data:
            stream = {}
            stream['description'] = 'MP4 audio/video'
            stream['id'] = media_format
            stream['url'] = media_data[media_format]['mediaURL']
            
            if media_format == "highDefinitionMP4":
                stream['width'] = 1280
                stream['height'] = 720
            else:
                stream['width'] = 640
                stream['height'] = 360
            
            streams.append(stream)
    except:
        pass
    
    result['items'] = streams
    return result

def get_user(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url))
        channel_info = page.find('section', attrs={'class': 'ChannelInfo'})
        thumbnail_url = page.find('meta', attrs={'property': 'og:image'})['content']
    
        if thumbnail_url[0] == '/':
            thumbnail_url = 'http://www.metacafe.com' + thumbnail_url
    
        user = {}
        user['description'] = channel_info.find('p').string
        user['id'] = url
        user['largeThumbnailUrl'] = thumbnail_url
        user['thumbnailUrl'] = thumbnail_url
        user['username'] = page.find('meta', attrs={'property': 'og:title'})['content']
        return user
    except:
        raise ResourceError('{"error": "Channel not found"}')    
    
def get_item(resource, id):
    if resource == 'video':
        return get_video(id)
    
    if resource == 'user':
        return get_user(id)
    
    return {}
        
def list_items(resource, id):
    if not resource or resource == 'video':
        return list_videos(id)
    
    if resource == 'stream':
        return list_streams(id)
    
    return []
        
def search_items(resoruce, query, order):
    if not resource or resource == 'video':
        return search_videos(query, order)
    
    return []
        
def main(method, resource, id, query, order):
    if method == 'list':
        print json.dumps(list_items(resource, id))
    elif method == 'search':
        print json.dumps(search_items(resource, query, order))
    elif method == 'get':
        print json.dumps(get_item(resource, id))
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
