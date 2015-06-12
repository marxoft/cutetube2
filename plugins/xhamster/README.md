#cuteTube2-XHamster

cuteTube2-XHamster is a plugin for cuteTube2 providing access to content from XHamster.

#Dependencies

python-beautifulsoup  
python-simplejson

#Usage

The following resources are supported:

<table>
    <tr>
        <th>Resource</th>
        <th>Methods</th>
    </tr>
    <tr>
        <td>video</td>
        <td>list, search, get</td>
    </tr>
    <tr>
        <td>user</td>
        <td>get</td>
    </tr>
    <tr>
        <td>category</td>
        <td>list</td>
    </tr>
    <tr>
        <td>comment</td>
        <td>list</td>
    </tr>
    <tr>
        <td>stream</td>
        <td>list</td>
    </tr>
</table>

##Examples

###Search for videos

    xhamster.py -m search -r video -q 'my query' -o rl

###List related videos
    
    xhamster.py -m list -r video -i http://xhamster.com/movies/1234/my_video

###Retrieve a video
    
    xhamster.py -m get -r video -i http://xhamster.com/movies/1234/my_video
    
###List video comments
    
    xhamster.py -m list -r comment -i http://xhamster.com/movies/1234/my_video

###List categories
    
    xhamster.py -m list -r category

###Retrieve a user channel
    
    xhamster.py -m get -r user -i http://xhamster.com/user/my_channel

###List video streams
    
    xhamster.py -m list -r stream -i http://xhamster.com/movies/1234/my_video
