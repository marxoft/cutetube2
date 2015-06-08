#About plugins

Plugins in cuteTube2 are used to provide access to additional services, such as videos from a website. Installed plugins 
are listed in the application, allowing users to select them and access the content which they provide.

#Creating a plugin

Creating a cuteTube2 plugin consists of three steps:

1. Defining the plugin using XML format in a \*.plugin file.
2. Defining the settings (if any) to be displayed in the UI using XML format.
3. Providing an executable to be called by the application.

##Defining a plugin

Plugins are defined using XML, in a file with \*.plugin extension. The plugin definition file can be place either in 
**/opt/cutetube2/plugins** or **~/.config/cuteTube2/plugins**.

The following nodes must be provided in a plugin definition file:

<table>
    <tr>
        <th>
            Name
        </th>
        <th>
            Description
        </th>
        <th>
            Attributes
        </th>
    </tr>
    <tr>
        <td>
            plugin
        </td>
        <td>
            The root node.
        </td>
        <td>
            <ul>
                <li><b>name</b> - The display name of the plugin.</li>
                <li><b>settings</b> - The path to the declarative settings file (absolute or relative path).</li>
                <li><b>exec</b> - The absolute path to the executable.</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            resources
        </td>
        <td>
            Encapsulates the resource declarations.
        </td>
        <td>
            None.
        </td>
    </tr>
    <tr>
        <td>
            resource
        </td>
        <td>
            Declares a supported resource.
        </td>
        <td>
            <ul>
                <li><b>method</b> - The method supported for this resource. Can be 'get', 'list' or 'search'.</li>
                <li><b>name</b> - The display name for the resource.</li>
                <li><b>type</b> - The resource type. Can be  'category', 'playlist', 'user' or 'video'.</li>
                <li><b>id</b> - An optional identifier to be passed as the <b>-i</b> argument when calling the 'list' method.</li>
                <li><b>order</b> - The value to be passed as the <b>-o</b> argument when calling the 'search' method.</li>
                <li><b>regexp</b> - The regular expression used to check if a URL is supported by the plugin. If a match 
                is found, the URL will be passed as the <b>-i</b> argument when calling the 'get' method.</li>
            </ul>
        </td>
    </tr>
</table>

###Example

Below is an example of a plugin definition for a plugin named **My Plugin**.

    <plugin name="My Plugin" settings="myplugin.settings" exec="/opt/cutetube2/plugins/myplugin">
        <resources>
            <resource method="list" name="Latest videos" type="video" id="latest_videos" />
            <resource method="list" name="Latest playlists" type="playlist" id="latest_playlists" />
            <resource method="list" name="Categories" type="category" id="all_categories" />
            <resource method="list" name="Users" type="user" id="all_users" />
            <resource method="search" name="Videos" type="video" order="relevance" />
            <resource method="search" name="Playlists" type="playlist" order="relevance" />
            <resource method="get" type="video" regexp="http://api.mywebsite.com/video/w+" />
            <resource method="get" type="playlist" regexp="http://api.mywebsite.com/playlist/w+" />
            <resource method="get" type="user" regexp="http://api.mywebsite.com/user/w+" />
        </resources>
    </plugin>

##Defining plugin settings

Plugin settings are defined using XML. The following types are supported:

<table>
    <tr>
        <th>
            Name
        </th>
        <th>
            Description
        </th>
        <th>
            Attributes
        </th>
    </tr>
    <tr>
        <td>
            boolean
        </td>
        <td>
            A boolean type with value true or false, represented in the UI by a checkbox or switch.
        </td>
        <td>
            <ul>
                <li><b>title</b> - The display name used in the text label.</li>
                <li><b>key</b> - The key used for storing the value.</li>
                <li><b>default</b> - The default value used when none has been set.</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            group
        </td>
        <td>
            A string providing a group heading, represented in the UI as a bold text label or similar.
        </td>
        <td>
            <ul>
                <li><b>title</b> - The display name used in the text label.</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            integer
        </td>
        <td>
            A number, represented in the UI by a spinbox or slider.
        </td>
        <td>
            <ul>
                <li><b>title</b> - The display name used in the text label.</li>
                <li><b>key</b> - The key used for storing the value.</li>
                <li><b>min</b> - The minimum value that can be set.</li>
                <li><b>max</b> - The maximium value that can be set.</li>
                <li><b>step</b> - The step size used when incrementing/decrementing the value.</li>
                <li><b>default</b> - The default value used when none has been set.</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            list
        </td>
        <td>
            An array of values from which a value can be chosen, represented in the UI by a combobox or similar.
        </td>
        <td>
            <ul>
                <li><b>title</b> - The display name used in the text label.</li>
                <li><b>key</b> - The key used for storing the value.</li>
                <li><b>default</b> - The default value used when none has been set.</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            element
        </td>
        <td>
            An element in a list. Can only be used as a child of a list.
        </td>
        <td>
            <ul>
                <li><b>name</b> - The display name.</li>
                <li><b>value</b> - The value.</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            text
        </td>
        <td>
            A string, represented in the user interface by a textfield.
        </td>
        <td>
            <ul>
                <li><b>title</b> - The display name used in the text label.</li>
                <li><b>key</b> - The key used for storing the value.</li>
                <li><b>default</b> - The default value used when none has been set.</li>
            </ul>
        </td>
    </tr>
</table>

###Example

Below is an example settings file for **My Plugin**.

    <settings title="My Plugin">
        <group title="Group one" />
        <boolean title="True or false" key="trueOrFalse" default="true" />
        <integer title="Pick a number from 1 to 5" key="oneToFive" min="1" max="5" step="1" default="1" />
        <group title="Group two" />
        <list title="Pick a number from 1 to 5" key="oneToTen" default="1">
            <element name="One" value="1" />
            <element name="Two" value="2" />
            <element name="Three" value="3" />
            <element name="Four" value="4" />
            <element name="Five" value="5" />
        </list>
        <text title="Enter a string" key="aString" default="Hello" />
    </settings>

##Providing an executable

The only requirement of a plugin executable is that is accepts certain arguments and writes an appropriate response 
to STDOUT in JSON format. It is therefore possible to create an executable using any language you prefer.

The following arguments are used when calling an executable:

<table>
    <tr>
        <th>
            Name
        </th>
        <th>
            Description
        </th>
        <th>
            Possible values
        </th>
    </tr>
    <tr>
        <td>
            -m
        </td>
        <td>
            The method called by the application.
        </td>
        <td>
            <ul>
                <li>get</li>
                <li>list</li>
                <li>search</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            -r
        </td>
        <td>
            The resource type requested by the application.
        </td>
        <td>
            <ul>
                <li>category</li>
                <li>playlist</li>
                <li>stream</li>
                <li>user</li>
                <li>video</li>
            </ul>
        </td>
    </tr>
    <tr>
        <td>
            -i
        </td>
        <td>
            The identifier for the requested resource. Used for calls to 'get' and 'list'.
        </td>
        <td>
            A string value. The value passed will be either one of those declared in the plugin definition file, or 
            one provided by the executable in the JSON response to a method call.
        </td>
    </tr>
    <tr>
        <td>
            -q
        </td>
        <td>
            The search query string. Used for calls to 'search'.
        </td>
        <td>
            A string value.
        </td>
    </tr>
    <tr>
        <td>
            -o
        </td>
        <td>
            The search sort order. Used for calls to 'search'.
        </td>
        <td>
            A string value. The value passed will be one of those declared in the plugin definition file.
        </td>
    </tr>
</table>

###Output format

cuteTube2 expects the executable to provide a response to method calls in JSON format, by writing the response to STDOUT. 
The values to be included in the response depend on both the method called and the resource type:

<table>
    <tr>
        <th>
            Method
        </th>
        <th>
            Response (success)
        </th>
        <th>
            Response (error)
        </th>
    </tr>
    <tr>
        <td>
            get
        </td>
        <td>
            { &lt;resource&gt; }
        </td>
        <td>
            { "error": &lt;error_string&gt; }
        </td>
    </tr>
    <tr>
        <td>
            list
        </td>
        <td>
            { "next": &lt;id_for_next_page_if_any&gt;, "items": &lt;array_of_resource&gt; }
        </td>
        <td>
            { "error": &lt;error_string&gt; }
        </td>
    </tr>
    <tr>
        <td>
            search
        </td>
        <td>
            { "next": &lt;id_for_next_page_if_any&gt;, "items": &lt;array_of_resource&gt; }
        </td>
        <td>
            { "error": &lt;error_string&gt; }
        </td>
    </tr>
</table>

##Resource types

###Category

<table>
    <tr>
        <th>
            Attribute
        </th>
        <th>
            Description
        </th>
    </tr>
    <tr>
        <td>
            id
        </td>
        <td>
            The resource identifier. This will be used to retrieve a list of videos belonging to the category.
        </td>
    </tr>
    <tr>
        <td>
            title
        </td>
        <td>
            The display title of the category.
        </td>
    </tr>
</table>

###Playlist

<table>
    <tr>
        <th>
            Attribute
        </th>
        <th>
            Description
        </th>
    </tr>
    <tr>
        <td>
            date
        </td>
        <td>
            The created date of the playlist formatted as a human-readable string.
        </td>
    </tr>
    <tr>
        <td>
            description
        </td>
        <td>
            The playlist description. Can be empty.
        </td>
    </tr>
    <tr>
        <td>
            id
        </td>
        <td>
            The resource identifier. This will be used to retrieve a list of videos belonging to the category.
        </td>
    </tr>
    <tr>
        <td>
            largeThumbnailUrl
        </td>
        <td>
            The URL to a large thumbnail. Re-use thumbnailUrl if no larger thumbnail is available.
        </td>
    </tr>
    <tr>
        <td>
            thumbnailUrl
        </td>
        <td>
            The URL to a thumbnail for the playlist.
        </td>
    </tr>
    <tr>
        <td>
            title
        </td>
        <td>
            The display title of the playlist.
        </td>
    </tr>
    <tr>
        <td>
            userId
        </td>
        <td>
            The id of the user that created the playlist. Can be empty.
        </td>
    </tr>
    <tr>
        <td>
            username
        </td>
        <td>
            The name of the user that created the playlist. Can be empty.
        </td>
    </tr>
</table>

###User

<table>
    <tr>
        <th>
            Attribute
        </th>
        <th>
            Description
        </th>
    </tr>
    <tr>
        <td>
            description
        </td>
        <td>
            The user description. Can be empty.
        </td>
    </tr>
    <tr>
        <td>
            id
        </td>
        <td>
            The resource identifier. This will be used to retrieve a list of videos belonging to the user.
        </td>
    </tr>
    <tr>
        <td>
            largeThumbnailUrl
        </td>
        <td>
            The URL to a large thumbnail. Re-use thumbnailUrl if no larger thumbnail is available.
        </td>
    </tr>
    <tr>
        <td>
            thumbnailUrl
        </td>
        <td>
            The URL to a thumbnail for the user.
        </td>
    </tr>
    <tr>
        <td>
            username
        </td>
        <td>
            The name of the user.
        </td>
    </tr>
</table>

###Stream

<table>
    <tr>
        <th>
            Attribute
        </th>
        <th>
            Description
        </th>
    </tr>
    <tr>
        <td>
            description
        </td>
        <td>
            The video stream description, for example the codec.
        </td>
    </tr>
    <tr>
        <td>
            height
        </td>
        <td>
            The height, in pixels, of the video stream.
        </td>
    </tr>
    <tr>
        <td>
            id
        </td>
        <td>
            The resource identifier.
        </td>
    </tr>
    <tr>
        <td>
            url
        </td>
        <td>
            The URL to the video stream, used for playback/download.
        </td>
    </tr>
    <tr>
        <td>
            width
        </td>
        <td>
            The width, in pixels, of the video stream.
        </td>
    </tr>
</table>

###Video

<table>
    <tr>
        <th>
            Attribute
        </th>
        <th>
            Description
        </th>
    </tr>
    <tr>
        <td>
            date
        </td>
        <td>
            The created date of the video formatted as a human-readable string.
        </td>
    </tr>
    <tr>
        <td>
            description
        </td>
        <td>
            The video description. Can be empty.
        </td>
    </tr>
    <tr>
        <td>
            duration
        </td>
        <td>
            The video duration in format [hours:]minutes:seconds.
        </td>
    </tr>
    <tr>
        <td>
            id
        </td>
        <td>
            The resource identifier. This will be used to retrieve a list of related videos.
        </td>
    </tr>
    <tr>
        <td>
            largeThumbnailUrl
        </td>
        <td>
            The URL to a large thumbnail. Re-use thumbnailUrl if no larger thumbnail is available.
        </td>
    </tr>
    <tr>
        <td>
            streamUrl
        </td>
        <td>
            The URL to the video stream. If specified, the application will use this when playing a video, instead of 
            requesting a list of streams.
        </td>
    </tr>
    <tr>
        <td>
            thumbnailUrl
        </td>
        <td>
            The URL to a thumbnail for the video.
        </td>
    </tr>
    <tr>
        <td>
            title
        </td>
        <td>
            The display title of the video.
        </td>
    </tr>
    <tr>
        <td>
            url
        </td>
        <td>
            The URL of the video. Used for sharing features.
        </td>
    </tr>
    <tr>
        <td>
            userId
        </td>
        <td>
            The id of the user that created the video. Can be empty.
        </td>
    </tr>
    <tr>
        <td>
            username
        </td>
        <td>
            The name of the user that created the video. Can be empty.
        </td>
    </tr>
</table>

###Examples

Method call:

    /opt/cutetube2/plugins/myplugin -m list -r category -i categories

Response:

    {
        "next": "http://api.mywebsite.com/categories?page=2",
        "items": [
            {
                "id": "abc123",
                "title": "Some category"
            },
            
            {
                "id": "xyz456",
                "title": "Another category"
            }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m get -r playlist -i http://api.mywebsite.com/playlist/abc123
    
Response:

    {
        "date": "08 Jun 2015",
        "description": "Some cool story about this playlist.",
        "id": "abc123",
        "largeThumbnailUrl": "http://static.mywebsite.com/images/abc123_large.jpg",
        "thumbnailUrl": "http://static.mywebsite.com/images/abc123.jpg",
        "title": "Some cool playlist",
        "url": "http://api.mywebsite.com/playlist/abc123",
        "userId": "xyz456",
        "username": "Some guy"
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m list -r playlist -i latest_playlists

Response:

    {
        "next": "http://api.mywebsite.com/latest_playlists?page=2",
        "items": [
            { <playlist_1> },
            { <playlist_2> }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m search -r playlists -q 'cool playlists' -o relevance

Response:

    {
        "next": "http://api.mywebsite.com/search?r=playlist&q=cool+playlists&o=relevance&page=2",
        "items": [
            { <playlist_1> },
            { <playlist_2> }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m list -r stream -i http://api.mywebsite.com/video/abc123

Response:

    {
        "items": [
            {
                "description": "HD MP4 stream",
                "height": 1280,
                "id": "abc123",
                "url": "http://static.mywebsite.com/streams/720/abc123.mp4",
                "width": 720
            },
            
            {
                "description": "SD MP4 stream",
                "height": 360,
                "id": "abc123",
                "url": "http://static.mywebsite.com/streams/360/abc123.mp4",
                "width": 640
            }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m get -r user -i http://api.mywebsite.com/user/abc123
    
Response:

    {
        "description": "Some cool story about this user.",
        "id": "abc123",
        "largeThumbnailUrl": "http://static.mywebsite.com/images/abc123_large.jpg",
        "thumbnailUrl": "http://static.mywebsite.com/images/abc123.jpg",
        "userId": "abc123",
        "username": "Some guy"
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m list -r user -i all_users

Response:

    {
        "next": "http://api.mywebsite.com/all_users?page=2",
        "items": [
            { <user_1> },
            { <user_2> }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m search -r user -q 'cool users' -o popular

Response:

    {
        "next": "http://api.mywebsite.com/search?r=user&q=cool+users&o=popular&page=2",
        "items": [
            { <user_1> },
            { <user_2> }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m get -r video -i http://api.mywebsite.com/video/abc123
    
Response:

    {
        "date": "08 Jun 2015",
        "description": "Some cool story about this video.",
        "duration": "05:00",
        "id": "abc123",
        "largeThumbnailUrl": "http://static.mywebsite.com/images/abc123_large.jpg",
        "streamUrl": "http://static.mywebsite.com/streams/abc123.mp4",
        "thumbnailUrl": "http://static.mywebsite.com/images/abc123.jpg",
        "title": "Some cool video",
        "url": "http://api.mywebsite.com/video/abc123",
        "userId": "xyz456",
        "username": "Some guy"
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m list -r video -i latest_videos

Response:

    {
        "next": "http://api.mywebsite.com/latest_videos?page=2",
        "items": [
            { <video_1> },
            { <video_2> }
        ]
    }

Method call:

    /opt/cutetube2/plugins/myplugin -m search -r video -q 'cool videos' -o relevance

Response:

    {
        "next": "http://api.mywebsite.com/search?r=video&q=cool+videos&o=relevance&page=2",
        "items": [
            { <video_1> },
            { <video_2> }
        ]
    }
