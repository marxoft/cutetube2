#cuteTube2-Pornhub

cuteTube2-Pornhub is a plugin for cuteTube2 providing access to content from Pornhub.

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
        <td>list, search, get</td>
    </tr>
    <tr>
        <td>category</td>
        <td>list</td>
    </tr>
    <tr>
        <td>stream</td>
        <td>list</td>
    </tr>
</table>

##Examples

###Search for videos

    pornhub.py -m search -r video -q 'my query' -o mr

###List related videos
    
    pornhub.py -m list -r video -i http://www.pornhub.com/view_video.php?viewkey=1234

###Retrieve a video
    
    pornhub.py -m get -r video -i http://www.pornhub.com/view_video.php?viewkey=1234

###List categories
    
    pornhub.py -m list -r category

###Retrieve a user channel
    
    pornhub.py -m get -r user -i http://pornhub.com/user/1234

###List video streams
    
    pornhub.py -m list -r stream -i http://www.pornhub.com/view_video.php?viewkey=1234
