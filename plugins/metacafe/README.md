#cuteTube2-Metacafe

cuteTube2-Metacafe is a plugin for cuteTube2 providing access to content from Metacafe.

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
        <td>stream</td>
        <td>list</td>
    </tr>
</table>

##Examples

###Search for videos

    metacafe.py -m search -r video -q 'my query' -o most_popular

###List related videos
    
    metacafe.py -m list -r video -i http://www.metacafe.com/watch/my_video

###Retrieve a video
    
    metacafe.py -m get -r video -i http://www.metacafe.com/watch/my_video

###Retrieve a user channel
    
    metacafe.py -m get -r user -i http://www.metacafe.com/channels/my_channel

###List video streams
    
    metacafe.py -m list -r stream -i http://www.metacafe.com/watch/my_video
