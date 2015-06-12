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
import urllib2
    
class ResourceError(Exception):
    pass

def format_date(date_string, date_format):    
    try:
        return datetime.strptime(date_string, date_format).strftime('%d %b %Y')
    except:
        return date_string

def get_page(url, headers = None):
    try:
        if headers is None:
            return urllib2.urlopen(urllib2.Request(url)).read()
        
        return urllib2.urlopen(urllib2.Request(url, None, headers)).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve page from %s"}' % url)
    
def list_videos(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    if url.find('/user/') > 0 and url.find('/video/') == -1:
        url = 'http://xhamster.com/user/video/' + url.split('/')[-1] + '/new-1.html'
    
    cookie = ''
    
    try:
        url_split = url.split('&cookie=')
        url = url_split[0]
        cookie = url_split[1].replace('\\', '')
    except:
        pass
    
    page = BeautifulSoup(get_page(url_split[0], {'Cookie': cookie}))
    result = {}
    videos = []
        
    for item in page.findAll('div', attrs={'class': 'video'}):
        try:
            video = {}

            try:
                video['duration'] = item.find('b').string
            except:
                pass

            video['id'] = item.find('a')['href']
            video['largeThumbnailUrl'] = item.find('img')['src']
            video['thumbnailUrl'] = video['largeThumbnailUrl']
            video['title'] = item.find('img')['alt']
            video['url'] = video['id']
    
            videos.append(video)
        except:
            pass
    
    try:
        try:
            result['next'] = page.find('a', attrs={'class': 'last colR'})['href'] + '&cookie=' + cookie
        except:
            result['next'] = page.find('div', attrs={'class': 'pager'}).find('a', attrs={'class': 'last'})['href'] + '&cookie=' + cookie
    except:
        pass
    
    result['items'] = videos
    return result
    
def search_videos(query, order):
    cookie = 'search_video=' + json.dumps({'sort': order, 'duration': '', 'channels': False, 'quality': 0, 'date': ''})
    headers = {'Cookie': cookie}
    page = BeautifulSoup(get_page("http://xhamster.com/search.php?qcat=video&q=" + query.replace(' ', '+'), headers).replace('\\', ''))
    result = {}
    videos = []
        
    for item in page.findAll('div', attrs={'class': 'video'}):
        try:
            video = {}

            try:
                video['duration'] = item.find('b').string
            except:
                pass

            video['id'] = item.find('a')['href']
            video['largeThumbnailUrl'] = item.find('img')['src']
            video['thumbnailUrl'] = video['largeThumbnailUrl']
            video['title'] = item.find('img')['alt']
            video['url'] = video['id']

            videos.append(video)
        except:
            pass
    
    try:
        result['next'] = page.find('div', attrs={'class': 'pager'}).find('a', attrs={'class': 'last'})['href'] + '&cookie=' + cookie
    except:
        pass
    
    result['items'] = videos
    return result

def get_video(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url))
        player = page.find('div', attrs={'id': 'playerBox'})
        stream = player.find('video')
        info = page.find('div', attrs={'id': 'videoInfoBox'})
        user = info.find('a')
    
        video = {}
        video['date'] = format_date(info.find('span', attrs={'class': 'hint'})['hint'].split(' ')[0], '%Y-%m-%d')
    
        try:
            video['description'] = info.find('h2').string
        except:
            pass
        
        video['duration'] = re.search(r'\d+:\d+', str(info.findAll('div', attrs={'class': 'item'})[1])).group(0)
        video['id'] = url
        video['largeThumbnailUrl'] = stream['poster']
        video['streamUrl'] = stream['file']
        video['thumbnailUrl'] = video['largeThumbnailUrl']
        video['title'] = player.find('h1').string
        video['url'] = url
        video['userId'] = user['href']
        video['username'] = user.string
        return video
    except:
        raise ResourceError('{"error": "Video not found"}')

def list_streams(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    result = {}
    streams = []
    
    try:
        page = BeautifulSoup(get_page(url))
        video = page.find('video')
        stream = {}
        stream['description'] = video['type']
        stream['height'] = 400
        stream['id'] = '0'
        stream['url'] = video['file']
        stream['width'] = 510
        streams.append(stream)
    except:
        pass
    
    result['items'] = streams
    return result

def list_categories():
    page = BeautifulSoup(get_page("http://xhamster.com/channels.php"))
    
    channels_list = page.find('div', attrs={'id': 'channelsList'})
    channel_group_titles = channels_list.findAll('div', attrs={'class': 'title'})
    channel_groups = channels_list.findAll('div', attrs={'class': 'list'})
    result = {}
    categories = []
    
    for i in range(len(channel_groups)):
        group_title = channel_group_titles[i].string
        
        for channel in channel_groups[i].findAll('a'):
            try:
                category = {}
                category['id'] = channel['href']
                category['title'] = group_title + ' - ' + re.search(r'[\w\s]+(?=</a>)', str(channel)).group(0).strip()
                categories.append(category)
            except:
                pass
    
    result['items'] = categories
    return result

def list_comments(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    page = BeautifulSoup(get_page(url))
    result = {}
    comments = []
    
    for item in page.find('div', attrs={'id': 'commentList'}).findAll('div', attrs={'class': 'item'}):
        try:
            user = item.find('a', attrs={'class': 'name'})
        
            comment = {}
            comment['body'] = item.find('div', attrs={'class': 'oh'}).string
            comment['date'] = item.find('div', attrs={'class': 'tool'}).find('span').string
            comment['id'] = item['cid']
            comment['thumbnailUrl'] = item.find('img')['src']
            comment['userId'] = user['href']
            comment['username'] = user.string
            comments.append(comment)
        except:
            pass
    
    result['items'] = comments
    return result

def get_user(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url))
        user = {}
        user['description'] = str(page.find('div', attrs={'class': 'about'})).split('about">')[1].split('</div')[0]
        user['id'] = url
        user['largeThumbnailUrl'] = page.find('table', attrs={'class': 'thumb'}).find('img')['src']
        user['thumbnailUrl'] = user['largeThumbnailUrl']
        user['username'] = page.find('tr', attrs={'class': 'title'}).find('h2').string
    except:
        raise ResourceError('{"error": "Profile not found"}')
    
    return user

def list_items(resource, id):
    if not resource or resource == 'video':
        return list_videos(id)
    
    if resource == 'stream':
        return list_streams(id)
    
    if resource == 'category':
        return list_categories()
    
    if resource == 'comment':
        return list_comments(id)
    
    return []

def get_item(resource, id):
    if not resource or resource == 'video':
        return get_video(id)
    
    if resource == 'user':
        return get_user(id)
    
    return {}
        
def search_items(resoruce, query, order):
    if not resource or resource == 'video':
        return search_videos(query, order)
    
    return []
        
def main(method, resource, id, query, order):
    if method == 'list':
        print json.dumps(list_items(resource, id))
    elif method == 'get':
        print json.dumps(get_item(resource, id))
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
