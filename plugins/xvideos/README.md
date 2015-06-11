#cuteTube2-XVideos

cuteTube2-XVideos is a plugin for cuteTube2 providing access to content from XVideos.

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
        <td>list, get</td>
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

    xvideos.py -m search -r video -q 'my query' -o most_popular

###List related videos
    
    xvideos.py -m list -r video -i http://www.xvideos.com/video1234/my_video

###Retrieve a video
    
    xvideos.py -m get -r video -i http://www.xvideos.com/video1234/my_video
    
###List users
    
    xvideos.py -m list -r user -i http://www.xvideos.com/profileslist

###Retrieve a user channel
    
    xvideos.py -m get -r user -i http://www.xvideos.com/profiles/my_channel

###List video streams
    
    xvideos.py -m list -r stream -i http://www.xvideos.com/video1234/my_video
