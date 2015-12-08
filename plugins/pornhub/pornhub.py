#!/usr/bin/env python

# Copyright (C) 2015 Stuart Howarth <showarth@marxoft.co.uk>
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

from BeautifulSoup import BeautifulSoup
import cgi
import getopt
import re
import sys
import urllib
import urllib2
import urlparse

try:
    import json
except ImportError:
    import simplejson as json

BASE_URL = 'http://www.pornhub.com'
    
class ResourceError(Exception):
    pass

def get_page(url, headers = None):
    try:
        if headers is None:
            return urllib2.urlopen(urllib2.Request(url)).read()
        
        return urllib2.urlopen(urllib2.Request(url, None, headers)).read()
    except:
        raise ResourceError('{"error": "Unable to retrieve page from %s"}' % url)

def increment_page(url):
    parsed = urlparse.urlparse(url)
    query = cgi.parse_qs(parsed.query)
    
    try:
        query['page'] = [str(int(query['page'][0]) + 1)]
    except:
        query['page'] = ['2']
    
    for q in query:
        query[q] = query[q][0]
    
    query_string = urllib.unquote(urllib.urlencode(query))
    return urlparse.urlunparse([parsed.scheme, parsed.netloc, parsed.path, parsed.params, query_string, parsed.fragment])

def get_redirect(url, headers = None):
    try:
        if headers is None:
            request = urllib2.Request(url)
        else:
            request = urllib2.Request(url, None, headers)

        request.get_method = lambda : 'HEAD'
        return urllib2.urlopen(request).geturl()
    except:
        return url

def list_videos(url):
    if not url:
        url = BASE_URL + '/video'
    
    result = {}
    videos = []
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=tablet'}))
        
        items = page.findAll('li', attrs={'class': 'videoblock'})

        if len(items) == 0:
            items = page.findAll('li', attrs={'class': 'videoblock mainUserChannels'})
        
        for item in items:
            try:            
                video = {}
                thumb = item.find('img', attrs={'class': 'thumb'})
                video['duration'] = item.find('span', attrs={'class': 'length'}).string
                video['id'] = BASE_URL + item.find('a', attrs={'class': 'img'})['href']
                video['largeThumbnailUrl'] = thumb['data-mediumthumb']
                video['thumbnailUrl'] = thumb['data-smallthumb']
                video['title'] = thumb['title']
                video['url'] = video['id']
                video['viewCount'] = item.find('p', attrs={'class': 'views'}).string.split(' ')[0].replace(',', '')
                videos.append(video)
            except:
                pass
    except:
        pass
    
    if len(videos) >= 20:
        result['next'] = increment_page(url)
    
    result['items'] = videos
    return result

def search_videos(query, order):
    url = '%s/video/search?search=%s' % (BASE_URL, query.replace(' ', '+'))
    
    if order != '':
        url += '&o=' + order
    
    return list_videos(url)

def get_video(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=mobile'}))
        video = {}
        video['duration'] = page.find('div', attrs={'class': 'duration thumbOverlay removeWhenPlaying details'}).string
        video['id'] = url
        video['largeThumbnailUrl'] = page.find('img', attrs={'class': 'mainImage'})['src']
        video['thumbnailUrl'] = page.find('meta', attrs={'property': 'og:image'})['content']
        video['title'] = page.find('div', attrs={'class': 'headerWrap sectionPadding clearfix'}).find('h1').string
        video['url'] = url
        
        try:
            user = page.find('div', attrs={'class': 'categoryRow parent clearfix'}).find('a')
            video['userId'] = BASE_URL + user['href']
            video['username'] = user.string
        except:
            pass
        
        video['viewCount'] = page.find('li', attrs={'class': 'views'}).find('span').string.replace(',', '')
        return video
    except:
        raise ResourceError('{"error": "Video not found"}')

def list_streams(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    result = {}
    streams = []
    
    try:
        page = get_page(url, {'Cookie': 'platform=mobile'})
        
        for quality in [240, 480, 720, 1080]:
            try:
                stream = {}
                stream['description'] = 'MP4 audio/video'
                stream['height'] = quality
                stream['id'] = str(quality)
                stream['url'] = get_redirect(re.search(r'(?<=player_quality_%dp = \')[^\']+' % quality, page).group(0))
                stream['width'] = stream['height'] * 4 / 3
                streams.append(stream)
            except:
                pass
    except:
        pass
    
    result['items'] = streams
    return result

def list_categories():
    result = {}
    categories = []
    
    try:
        page = BeautifulSoup(get_page(BASE_URL + '/categories', {'Cookie': 'platform=mobile'}))
        
        for item in page.find('ul', attrs={'id': 'fullList'}).findAll('li'):
            try:
                category = {}
                cat_pic = item.find('a', attrs={'class': 'catPic'})
                category['id'] = BASE_URL + cat_pic['href'] + '&o=cm'
                category['title'] = cat_pic.find('img')['alt']
                categories.append(category)
            except:
                pass
    except:
        pass
    
    result['items'] = categories
    return result

def list_channels(url):
    if not url:
        url = BASE_URL + '/channels?o=rk'
    
    result = {}
    channels = []
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=tablet'}))
        
        for item in page.find('div', attrs={'class': 'channelsContainer clearfix'}).findAll('li', attrs={'class': 'wrap'}):
            try:
                channel = {}
                avatar = item.find('div', attrs={'class': 'avatar'})
                channel['id'] = BASE_URL + avatar.find('a')['href'] + '/videos?o=da'
                channel['largeThumbnailUrl'] = avatar.find('img')['src']
                channel['thumbnailUrl'] = channel['largeThumbnailUrl']
                channel['username'] = item.find('a', attrs={'class': 'usernameLink'}).string
                channels.append(channel)
            except:
                pass
    except:
        pass

    if len(channels) >= 36:
        result['next'] = increment_page(url)
    
    result['items'] = channels
    return result

def list_members(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    result = {}
    members = []
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=tablet'}))
        
        for item in page.find('ul', attrs={'class': 'userWidgetWrapperGrid  search-results'}).findAll('li'):
            try:
                member = {}
                username_link = item.find('a', attrs={'class': 'showName'})
                member['id'] = BASE_URL + username_link['href'] + '/videos/public'
                member['largeThumbnailUrl'] = item.find('img', attrs={'class': 'avatar'})['src']
                member['thumbnailUrl'] = member['largeThumbnailUrl']
                member['username'] = username_link.string
                members.append(member)
            except:
                pass
    except:
        pass
    
    if len(members) >= 42:
        result['next'] = increment_page(url)
    
    result['items'] = members
    return result

def list_pornstars(url):
    if not url:
        url = BASE_URL + '/pornstars'
    
    result = {}
    pornstars = []
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=tablet'}))
        
        for item in page.find('ul', attrs={'class': 'pornstarListing page1'}).findAll('li'):
            try:
                pornstar = {}
                name = item.find('a', attrs={'class': 'name'})
                pornstar['id'] = BASE_URL + name['href'] + '?o=cm'
                pornstar['largeThumbnailUrl'] = item.find('img')['src']
                pornstar['thumbnailUrl'] = pornstar['largeThumbnailUrl']
                pornstar['username'] = name.string
                pornstars.append(pornstar)
            except:
                pass
    except:
        pass
    
    if len(pornstars) >= 18:
        result['next'] = increment_page(url)
    
    result['items'] = pornstars
    return result

def list_users(url):
    if not url or url.find('/pornstars') != -1:
        return list_pornstars(url)

    if url.find('/channels') != -1:
        return list_channels(url)
    
    return list_members(url)

def search_users(query, order):
    try:
        user_type, order = order.split('_')
    except:
        user_type = 'pornstars'
        order = ''
    
    if user_type == 'pornstars':
        url = '%s/pornstars/search?search=%s' % (BASE_URL, query.replace(' ', '+'))
    else:
        url = '%s/user/search?username=%s' % (BASE_URL, query.replace(' ', '+'))
    
    if order != '':
        url += '&o=' + order
    
    return list_users(url)

def get_channel(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=mobile'}))
        channel = {}
        channel['description'] = re.sub(r'<[^>]*>', '', str(page.find('div', attrs={'class': 'content'})))
        channel['id'] = url + '/videos?o=da'
        channel['largeThumbnailUrl'] = page.find('div', attrs={'class': 'avatar'}).find('img')['src']
        channel['thumbnailUrl'] = channel['largeThumbnailUrl']
        channel['username'] = page.find('div', attrs={'class': 'channelName floatLeft'}).find('h3').string
        return channel
    except:
        raise ResourceError('{"error": "No profile found"}')

def get_member(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=mobile'}))
        member = {}
        username = page.find('a', attrs={'class': 'username'})
        member['description'] = re.sub(r'<[^>]*>', '', str(page.find('div', attrs={'id': 'infoVisible'})))
        member['id'] = BASE_URL + username['href'] + '/videos/public'
        member['largeThumbnailUrl'] = page.find('img', attrs={'id': 'getAvatar'})['src']
        member['thumbnailUrl'] = member['largeThumbnailUrl']
        member['username'] = username.string
        return member
    except:
        raise ResourceError('{"error": "No profile found"}')

def get_pornstar(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    try:
        page = BeautifulSoup(get_page(url, {'Cookie': 'platform=mobile'}))
        pornstar = {}
        pornstar['description'] = re.sub(r'<[^>]*>', '', str(page.find('div', attrs={'id': 'bioBlock'})))
        pornstar['id'] = url + '?o=cm'
        pornstar['largeThumbnailUrl'] = page.find('img', attrs={'itemprop': 'contentUrl'})['src']
        pornstar['thumbnailUrl'] = pornstar['largeThumbnailUrl']
        pornstar['username'] = page.find('span', attrs={'itemprop': 'name'}).string.strip().rstrip()
        return pornstar
    except:
        raise ResourceError('{"error": "No profile found"}')

def get_user(url):
    if not url:
        raise ResourceError('{"error": "No URL specified"}')
    
    if url.find('/pornstar/') != -1:
        return get_pornstar(url)

    if url.find('/channels/') != -1:
        return get_channel(url)
    
    return get_member(url)

def list_items(resource, id):
    if not resource or resource == 'video':
        return list_videos(id)
    
    if resource == 'stream':
        return list_streams(id)
    
    if resource == 'category':
        return list_categories()
    
    if resource == 'user':
        return list_users(id)
    
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
    
    if resource == 'user':
        return search_users(query, order)
    
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
