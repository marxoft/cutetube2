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
import getopt
import re
import simplejson as json
import sys
import urllib
    
class ResourceError(Exception):
    pass
        
def get_page(url):
    try:
        return urllib.urlopen(url).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve page from %s"}' % url)
    
def list_videos(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    if url.find('/profiles/') and url.find('/videos') is -1:
        if url[-1] is not '/':
            url += '/'
        
        url += 'videos'
        
    page = BeautifulSoup(get_page(url))
    result = {}
    videos = []
    
    html = str(page)
    
    if html.find('videoPageWriteRelated') > 0:
        related = re.sub(r'((?<=\b)(\w{1,2})(?=:))', r'"\1"', re.search(r'(?<=videoPageWriteRelated\().+(?=\))', html).group(0))
        lists = json.loads(related)
        
        for items in lists:
            for item in items:
                video = {}
                video['duration'] = re.search(r'\d+', item['d']).group(0) + ':00'
                video['id'] = 'http://www.xvideos.com' + item['u']
                video['largeThumbnailUrl'] = item['i']
                video['thumbnailUrl'] = item['i']
                video['title'] = item['t']
                video['url'] = video['id']
                videos.append(video)
    else:
        for item in page.findAll('div', attrs={'class': 'thumbInside'}):
            try:
                video = {}
        
                try:
                    video['duration'] = re.search(r'\d+', item.find('span', attrs={'class': 'duration'}).string).group(0) + ':00'
                except:
                    pass
                
                anchor = item.findAll('a')[-1]
        
                video['id'] = 'http://www.xvideos.com' + anchor['href']
                video['largeThumbnailUrl'] = item.find('img')['src']
                video['thumbnailUrl'] = video['largeThumbnailUrl']
                video['title'] = anchor['title']
                video['url'] = video['id']
                videos.append(video)
            except:
                pass
    
        try:
            result['next'] = 'http://www.xvideos.com' + page.findAll('a', attrs={'class': 'nP'})[-1]['href']
        except:
            pass
    
    result['items'] = videos
    return result
    
def search_videos(query, order):
    return list_videos("http://www.xvideos.com/?k=%s&sort=%s" % (query.replace(' ', '+'), order))

def get_video(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url))
        main = page.find('div', attrs={'id': 'main'})
        player = page.find('embed', attrs={'id': 'flash-player-embed'})
        user = main.find('li', attrs={'class': 'profile_name'}).find('a')
    
        video = {}
        video['duration'] = re.search(r'\d+', main.find('span', attrs={'class': 'duration'}).string).group(0) + ':00'
        video['id'] = url
        video['largeThumbnailUrl'] = re.search(r'(?<=url_bigthumb=)[^&]+', player['flashvars']).group(0)
        video['streamUrl'] = urllib.unquote(re.search(r'(?<=flv_url=)[^&]+', player['flashvars']).group(0))
        video['thumbnailUrl'] = video['largeThumbnailUrl']
        video['title'] = re.search(r'(?<=>)[^<]+', str(main.find('h2'))).group(0).strip()
        video['url'] = url
        video['userId'] = 'http://www.xvideos.com/profiles/' + user['href'].split('/')[-1]
        video['username'] = user.string
        return video
    except:
        raise ResourceError('{"error": "Video not found"}')
    
def list_users(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    page = BeautifulSoup(get_page(url))
    result = {}
    users = []
        
    for item in page.findAll('div', attrs={'class': 'thumbProfile'}):
        try:
            user = {}
            user['id'] = 'http://www.xvideos.com/profiles/' + item.find('a')['href'].split('/')[-1]
            user['largeThumbnailUrl'] = item.find('img')['src']
            user['thumbnailUrl'] = user['largeThumbnailUrl']
            user['title'] = item.find('p', attrs={'class': 'profileName'}).find('a').string
            user['username'] = user['title']
            users.append(user)
        except:
            pass
    
    try:
        result['next'] = 'http://www.xvideos.com' + page.findAll('a', attrs={'class': 'nP'})[-1]['href']
    except:
        pass
    
    result['items'] = users
    return result

def get_user(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url))
        main = page.find('div', attrs={'id': 'main'})

        user = {}
        user['description'] = main.find('p', attrs={'class': 'sexAgeCountry'}).find('span').string.strip()
        user['id'] = 'http://www.xvideos.com/profiles/' + url.split('/')[-1]
        user['largeThumbnailUrl'] = main.find('div', attrs={'id': 'profilePic'}).find('img')['src']
        user['thumbnailUrl'] = user['largeThumbnailUrl']
        user['username'] = re.search(r'(?<=>)[^<]+', str(main.find('h2'))).group(0).strip()
        return user
    except:
        raise ResourceError('{"error": "Profile not found"}')

def list_categories(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    return list_users(url)

def list_streams(id):
    result = {}
    streams = []
    
    try:
        page = BeautifulSoup(get_page(id))
        video = page.find('embed', attrs={'id': 'flash-player-embed'})
        stream = {}
        stream['description'] = video['quality']
        stream['height'] = video['height']
        stream['id'] = '0'
        stream['url'] = urllib.unquote(re.search(r'(?<=flv_url=)[^&]+', video['flashvars']).group(0))
        stream['width'] = video['width']
        streams.append(stream)
    except:
        pass
    
    result['items'] = streams
    return result

def get_item(resource, id):
    if resource == 'video':
        return get_video(id)
    
    if resource == 'user':
        return get_user(id)
    
    return {}
        
def list_items(resource, id):
    if not resource or resource == 'video':
        return list_videos(id)
    
    if resource == 'user':
        return list_users(id)
    
    if resource == 'category':
        return list_categories(id)
    
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
