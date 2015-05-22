TEMPLATE = app
TARGET = cutetube2

#DEFINES += CUTETUBE_DEBUG

QT += network sql

INCLUDEPATH += \
    src/base \
    src/dailymotion \
    src/plugins \
    src/vimeo \
    src/youtube

HEADERS += \
    src/base/categorymodel.h \
    src/base/categorynamemodel.h \
    src/base/comment.h \
    src/base/concurrenttransfersmodel.h \
    src/base/database.h \
    src/base/definitions.h \
    src/base/json.h \
    src/base/networkproxytypemodel.h \
    src/base/playlist.h \
    src/base/resources.h \
    src/base/searchhistorymodel.h \
    src/base/selectionmodel.h \
    src/base/servicemodel.h \
    src/base/settings.h \
    src/base/transfer.h \
    src/base/transfers.h \
    src/base/user.h \
    src/base/utils.h \
    src/base/video.h \
    src/base/videomodel.h \
    src/base/videoplayer.h \
    src/dailymotion/dailymotion.h \
    src/dailymotion/dailymotionaccountmodel.h \
    src/dailymotion/dailymotioncomment.h \
    src/dailymotion/dailymotioncommentmodel.h \
    src/dailymotion/dailymotionnavmodel.h \
    src/dailymotion/dailymotionplaylist.h \
    src/dailymotion/dailymotionplaylistmodel.h \
    src/dailymotion/dailymotionsearchordermodel.h \
    src/dailymotion/dailymotionsearchtypemodel.h \
    src/dailymotion/dailymotionstreammodel.h \
    src/dailymotion/dailymotionsubtitlemodel.h \
    src/dailymotion/dailymotionuser.h \
    src/dailymotion/dailymotionusermodel.h \
    src/dailymotion/dailymotionvideo.h \
    src/dailymotion/dailymotionvideomodel.h \
    src/plugins/plugincomment.h \
    src/plugins/plugincommentmodel.h \
    src/plugins/pluginnavmodel.h \
    src/plugins/pluginplaylist.h \
    src/plugins/pluginplaylistmodel.h \
    src/plugins/pluginsettingsmodel.h \
    src/plugins/resourcesplugins.h \
    src/plugins/resourcesrequest.h \
    src/plugins/pluginsearchordermodel.h \
    src/plugins/pluginsearchtypemodel.h \
    src/plugins/pluginstreammodel.h \
    src/plugins/pluginuser.h \
    src/plugins/pluginusermodel.h \
    src/plugins/pluginvideo.h \
    src/plugins/pluginvideomodel.h \
    src/vimeo/vimeo.h \
    src/vimeo/vimeoaccountmodel.h \
    src/vimeo/vimeocomment.h \
    src/vimeo/vimeocommentmodel.h \
    src/vimeo/vimeonavmodel.h \
    src/vimeo/vimeoplaylist.h \
    src/vimeo/vimeoplaylistmodel.h \
    src/vimeo/vimeosearchordermodel.h \
    src/vimeo/vimeosearchtypemodel.h \
    src/vimeo/vimeostreammodel.h \
    src/vimeo/vimeosubtitlemodel.h \
    src/vimeo/vimeouser.h \
    src/vimeo/vimeousermodel.h \
    src/vimeo/vimeovideo.h \
    src/vimeo/vimeovideomodel.h \
    src/youtube/youtube.h \
    src/youtube/youtubeaccountmodel.h \
    src/youtube/youtubecomment.h \
    src/youtube/youtubecommentmodel.h \
    src/youtube/youtubenavmodel.h \
    src/youtube/youtubeplaylist.h \
    src/youtube/youtubeplaylistmodel.h \
    src/youtube/youtubesearchordermodel.h \
    src/youtube/youtubesearchtypemodel.h \
    src/youtube/youtubestreammodel.h \
    src/youtube/youtubesubtitlemodel.h \
    src/youtube/youtubeuser.h \
    src/youtube/youtubeusermodel.h \
    src/youtube/youtubevideo.h \
    src/youtube/youtubevideomodel.h
    
SOURCES += \
    src/base/categorymodel.cpp \
    src/base/comment.cpp \
    src/base/json.cpp \
    src/base/playlist.cpp \
    src/base/resources.cpp \
    src/base/searchhistorymodel.cpp \
    src/base/selectionmodel.cpp \
    src/base/settings.cpp \
    src/base/transfer.cpp \
    src/base/transfers.cpp \
    src/base/user.cpp \
    src/base/utils.cpp \
    src/base/video.cpp \
    src/base/videomodel.cpp \
    src/base/videoplayer.cpp \
    src/dailymotion/dailymotion.cpp \
    src/dailymotion/dailymotionaccountmodel.cpp \
    src/dailymotion/dailymotioncomment.cpp \
    src/dailymotion/dailymotioncommentmodel.cpp \
    src/dailymotion/dailymotionnavmodel.cpp \
    src/dailymotion/dailymotionplaylist.cpp \
    src/dailymotion/dailymotionplaylistmodel.cpp \
    src/dailymotion/dailymotionstreammodel.cpp \
    src/dailymotion/dailymotionsubtitlemodel.cpp \
    src/dailymotion/dailymotionuser.cpp \
    src/dailymotion/dailymotionusermodel.cpp \
    src/dailymotion/dailymotionvideo.cpp \
    src/dailymotion/dailymotionvideomodel.cpp \
    src/plugins/plugincomment.cpp \
    src/plugins/plugincommentmodel.cpp \
    src/plugins/pluginplaylist.cpp \
    src/plugins/pluginplaylistmodel.cpp \
    src/plugins/resourcesplugins.cpp \
    src/plugins/resourcesrequest.cpp \
    src/plugins/pluginstreammodel.cpp \
    src/plugins/pluginusermodel.cpp \
    src/plugins/pluginuser.cpp \
    src/plugins/pluginvideomodel.cpp \
    src/plugins/pluginvideo.cpp \
    src/vimeo/vimeo.cpp \
    src/vimeo/vimeoaccountmodel.cpp \
    src/vimeo/vimeocomment.cpp \
    src/vimeo/vimeocommentmodel.cpp \
    src/vimeo/vimeonavmodel.cpp \
    src/vimeo/vimeoplaylist.cpp \
    src/vimeo/vimeoplaylistmodel.cpp \
    src/vimeo/vimeostreammodel.cpp \
    src/vimeo/vimeosubtitlemodel.cpp \
    src/vimeo/vimeouser.cpp \
    src/vimeo/vimeousermodel.cpp \
    src/vimeo/vimeovideo.cpp \
    src/vimeo/vimeovideomodel.cpp \
    src/youtube/youtube.cpp \
    src/youtube/youtubeaccountmodel.cpp \
    src/youtube/youtubecomment.cpp \
    src/youtube/youtubecommentmodel.cpp \
    src/youtube/youtubenavmodel.cpp \
    src/youtube/youtubeplaylist.cpp \
    src/youtube/youtubeplaylistmodel.cpp \
    src/youtube/youtubestreammodel.cpp \
    src/youtube/youtubesubtitlemodel.cpp \
    src/youtube/youtubeuser.cpp \
    src/youtube/youtubeusermodel.cpp \
    src/youtube/youtubevideo.cpp \
    src/youtube/youtubevideomodel.cpp

maemo5 {
    DEFINES += NAV_SEARCH
    
    LIBS += -L/usr/lib -lqdailymotion -lqvimeo -lqyoutube
    CONFIG += link_prl
    PKGCONFIG += libqdailymotion libqvimeo libqyoutube
    
    QT += maemo5 webkit
    CONFIG += mobility12
    MOBILITY += multimedia
    
    INCLUDEPATH += \
        src/imagecache \
        src/maemo5 \
        src/maemo5/dailymotion \
        src/maemo5/plugins \
        src/maemo5/vimeo \
        src/maemo5/youtube
    
    HEADERS += \
        src/base/transfermodel.h \
        src/base/transferprioritymodel.h \
        src/imagecache/imagecache.h \
        src/maemo5/aboutdialog.h \
        src/maemo5/banner.h \
        src/maemo5/categoriesdialog.h \
        src/maemo5/dialog.h \
        src/maemo5/filterbox.h \
        src/maemo5/image.h \
        src/maemo5/listview.h \
        src/maemo5/mainwindow.h \
        src/maemo5/navdelegate.h \
        src/maemo5/networkproxydialog.h \
        src/maemo5/newcategorydialog.h \
        src/maemo5/playlistthumbnail.h \
        src/maemo5/qwebviewselectionsuppressor.h \
        src/maemo5/screensaver.h \
        src/maemo5/searchhistorydialog.h \
        src/maemo5/settingsdialog.h \
        src/maemo5/stackedwindow.h \
        src/maemo5/textbrowser.h \
        src/maemo5/transferswindow.h \
        src/maemo5/valueselector.h \
        src/maemo5/valueselectoraction.h \
        src/maemo5/videocontrols.h \
        src/maemo5/videoplaybackdelegate.h \
        src/maemo5/videoplaybackwindow.h \
        src/maemo5/videoplayerbutton.h \
        src/maemo5/videothumbnail.h \
        src/maemo5/videowidget.h \
        src/maemo5/webview.h \
        src/maemo5/dailymotion/dailymotionaccountdelegate.h \
        src/maemo5/dailymotion/dailymotionaccountswindow.h \
        src/maemo5/dailymotion/dailymotionauthdialog.h \
        src/maemo5/dailymotion/dailymotioncommentdelegate.h \
        src/maemo5/dailymotion/dailymotioncommentdialog.h \
        src/maemo5/dailymotion/dailymotiondownloaddialog.h \
        src/maemo5/dailymotion/dailymotionplaybackdialog.h \
        src/maemo5/dailymotion/dailymotionplaylistdelegate.h \
        src/maemo5/dailymotion/dailymotionplaylistdialog.h \
        src/maemo5/dailymotion/dailymotionplaylistswindow.h \
        src/maemo5/dailymotion/dailymotionplaylistwindow.h \
        src/maemo5/dailymotion/dailymotionsearchdialog.h \
        src/maemo5/dailymotion/dailymotionuserdelegate.h \
        src/maemo5/dailymotion/dailymotionuserswindow.h \
        src/maemo5/dailymotion/dailymotionuserwindow.h \
        src/maemo5/dailymotion/dailymotionvideodelegate.h \
        src/maemo5/dailymotion/dailymotionvideoswindow.h \
        src/maemo5/dailymotion/dailymotionvideowindow.h \
        src/maemo5/dailymotion/dailymotionview.h \
        src/maemo5/plugins/plugincommentdelegate.h \
        src/maemo5/plugins/plugindownloaddialog.h \
        src/maemo5/plugins/pluginplaybackdialog.h \
        src/maemo5/plugins/pluginplaylistdelegate.h \
        src/maemo5/plugins/pluginplaylistswindow.h \
        src/maemo5/plugins/pluginplaylistwindow.h \
        src/maemo5/plugins/pluginsearchdialog.h \
        src/maemo5/plugins/pluginsettingscheckbox.h \
        src/maemo5/plugins/pluginsettingsdialog.h \
        src/maemo5/plugins/pluginsettingslineedit.h \
        src/maemo5/plugins/pluginsettingsselector.h \
        src/maemo5/plugins/pluginsettingsslider.h \
        src/maemo5/plugins/pluginsettingsspinbox.h \
        src/maemo5/plugins/pluginuserdelegate.h \
        src/maemo5/plugins/pluginuserswindow.h \
        src/maemo5/plugins/pluginuserwindow.h \
        src/maemo5/plugins/pluginvideodelegate.h \
        src/maemo5/plugins/pluginvideoswindow.h \
        src/maemo5/plugins/pluginvideowindow.h \
        src/maemo5/plugins/pluginview.h \
        src/maemo5/vimeo/vimeoaccountdelegate.h \
        src/maemo5/vimeo/vimeoaccountswindow.h \
        src/maemo5/vimeo/vimeoauthdialog.h \
        src/maemo5/vimeo/vimeocommentdelegate.h \
        src/maemo5/vimeo/vimeocommentdialog.h \
        src/maemo5/vimeo/vimeodownloaddialog.h \
        src/maemo5/vimeo/vimeoplaybackdialog.h \
        src/maemo5/vimeo/vimeoplaylistdelegate.h \
        src/maemo5/vimeo/vimeoplaylistdialog.h \
        src/maemo5/vimeo/vimeoplaylistswindow.h \
        src/maemo5/vimeo/vimeoplaylistwindow.h \
        src/maemo5/vimeo/vimeosearchdialog.h \
        src/maemo5/vimeo/vimeouserdelegate.h \
        src/maemo5/vimeo/vimeouserswindow.h \
        src/maemo5/vimeo/vimeouserwindow.h \
        src/maemo5/vimeo/vimeovideodelegate.h \
        src/maemo5/vimeo/vimeovideoswindow.h \
        src/maemo5/vimeo/vimeovideowindow.h \
        src/maemo5/vimeo/vimeoview.h \
        src/maemo5/youtube/youtubeaccountdelegate.h \
        src/maemo5/youtube/youtubeaccountswindow.h \
        src/maemo5/youtube/youtubeauthdialog.h \
        src/maemo5/youtube/youtubecommentdelegate.h \
        src/maemo5/youtube/youtubecommentdialog.h \
        src/maemo5/youtube/youtubedownloaddialog.h \
        src/maemo5/youtube/youtubeplaybackdialog.h \
        src/maemo5/youtube/youtubeplaylistdelegate.h \
        src/maemo5/youtube/youtubeplaylistdialog.h \
        src/maemo5/youtube/youtubeplaylistswindow.h \
        src/maemo5/youtube/youtubeplaylistwindow.h \
        src/maemo5/youtube/youtubesearchdialog.h \
        src/maemo5/youtube/youtubeuserdelegate.h \
        src/maemo5/youtube/youtubeuserswindow.h \
        src/maemo5/youtube/youtubeuserwindow.h \
        src/maemo5/youtube/youtubevideodelegate.h \
        src/maemo5/youtube/youtubevideoswindow.h \
        src/maemo5/youtube/youtubevideowindow.h \
        src/maemo5/youtube/youtubeview.h
        
    SOURCES += \
        src/base/transfermodel.cpp \
        src/imagecache/imagecache.cpp \
        src/maemo5/aboutdialog.cpp \
        src/maemo5/banner.cpp \
        src/maemo5/categoriesdialog.cpp \
        src/maemo5/dialog.cpp \
        src/maemo5/filterbox.cpp \
        src/maemo5/image.cpp \
        src/maemo5/listview.cpp \
        src/maemo5/main.cpp \
        src/maemo5/mainwindow.cpp \
        src/maemo5/navdelegate.cpp \
        src/maemo5/networkproxydialog.cpp \
        src/maemo5/newcategorydialog.cpp \
        src/maemo5/playlistthumbnail.cpp \
        src/maemo5/screensaver.cpp \
        src/maemo5/searchhistorydialog.cpp \
        src/maemo5/settingsdialog.cpp \
        src/maemo5/stackedwindow.cpp \
        src/maemo5/textbrowser.cpp \
        src/maemo5/transferswindow.cpp \
        src/maemo5/valueselector.cpp \
        src/maemo5/valueselectoraction.cpp \
        src/maemo5/videocontrols.cpp \
        src/maemo5/videoplaybackdelegate.cpp \
        src/maemo5/videoplaybackwindow.cpp \
        src/maemo5/videoplayerbutton.cpp \
        src/maemo5/videothumbnail.cpp \
        src/maemo5/videowidget.cpp \
        src/maemo5/webview.cpp \
        src/maemo5/dailymotion/dailymotionaccountdelegate.cpp \
        src/maemo5/dailymotion/dailymotionaccountswindow.cpp \
        src/maemo5/dailymotion/dailymotionauthdialog.cpp \
        src/maemo5/dailymotion/dailymotioncommentdelegate.cpp \
        src/maemo5/dailymotion/dailymotioncommentdialog.cpp \
        src/maemo5/dailymotion/dailymotiondownloaddialog.cpp \
        src/maemo5/dailymotion/dailymotionplaybackdialog.cpp \
        src/maemo5/dailymotion/dailymotionplaylistdelegate.cpp \
        src/maemo5/dailymotion/dailymotionplaylistdialog.cpp \
        src/maemo5/dailymotion/dailymotionplaylistswindow.cpp \
        src/maemo5/dailymotion/dailymotionplaylistwindow.cpp \
        src/maemo5/dailymotion/dailymotionsearchdialog.cpp \
        src/maemo5/dailymotion/dailymotionuserdelegate.cpp \
        src/maemo5/dailymotion/dailymotionuserswindow.cpp \
        src/maemo5/dailymotion/dailymotionuserwindow.cpp \
        src/maemo5/dailymotion/dailymotionvideodelegate.cpp \
        src/maemo5/dailymotion/dailymotionvideoswindow.cpp \
        src/maemo5/dailymotion/dailymotionvideowindow.cpp \
        src/maemo5/dailymotion/dailymotionview.cpp \
        src/maemo5/plugins/plugincommentdelegate.cpp \
        src/maemo5/plugins/plugindownloaddialog.cpp \
        src/maemo5/plugins/pluginplaybackdialog.cpp \
        src/maemo5/plugins/pluginplaylistdelegate.cpp \
        src/maemo5/plugins/pluginplaylistswindow.cpp \
        src/maemo5/plugins/pluginplaylistwindow.cpp \
        src/maemo5/plugins/pluginsearchdialog.cpp \
        src/maemo5/plugins/pluginsettingscheckbox.cpp \
        src/maemo5/plugins/pluginsettingsdialog.cpp \
        src/maemo5/plugins/pluginsettingslineedit.cpp \
        src/maemo5/plugins/pluginsettingsselector.cpp \
        src/maemo5/plugins/pluginsettingsslider.cpp \
        src/maemo5/plugins/pluginsettingsspinbox.cpp \
        src/maemo5/plugins/pluginuserdelegate.cpp \
        src/maemo5/plugins/pluginuserswindow.cpp \
        src/maemo5/plugins/pluginuserwindow.cpp \
        src/maemo5/plugins/pluginvideodelegate.cpp \
        src/maemo5/plugins/pluginvideoswindow.cpp \
        src/maemo5/plugins/pluginvideowindow.cpp \
        src/maemo5/plugins/pluginview.cpp \
        src/maemo5/vimeo/vimeoaccountdelegate.cpp \
        src/maemo5/vimeo/vimeoaccountswindow.cpp \
        src/maemo5/vimeo/vimeoauthdialog.cpp \
        src/maemo5/vimeo/vimeocommentdelegate.cpp \
        src/maemo5/vimeo/vimeocommentdialog.cpp \
        src/maemo5/vimeo/vimeodownloaddialog.cpp \
        src/maemo5/vimeo/vimeoplaybackdialog.cpp \
        src/maemo5/vimeo/vimeoplaylistdelegate.cpp \
        src/maemo5/vimeo/vimeoplaylistdialog.cpp \
        src/maemo5/vimeo/vimeoplaylistswindow.cpp \
        src/maemo5/vimeo/vimeoplaylistwindow.cpp \
        src/maemo5/vimeo/vimeosearchdialog.cpp \
        src/maemo5/vimeo/vimeouserdelegate.cpp \
        src/maemo5/vimeo/vimeouserswindow.cpp \
        src/maemo5/vimeo/vimeouserwindow.cpp \
        src/maemo5/vimeo/vimeovideodelegate.cpp \
        src/maemo5/vimeo/vimeovideoswindow.cpp \
        src/maemo5/vimeo/vimeovideowindow.cpp \
        src/maemo5/vimeo/vimeoview.cpp \
        src/maemo5/youtube/youtubeaccountdelegate.cpp \
        src/maemo5/youtube/youtubeaccountswindow.cpp \
        src/maemo5/youtube/youtubeauthdialog.cpp \
        src/maemo5/youtube/youtubecommentdelegate.cpp \
        src/maemo5/youtube/youtubecommentdialog.cpp \
        src/maemo5/youtube/youtubedownloaddialog.cpp \
        src/maemo5/youtube/youtubeplaybackdialog.cpp \
        src/maemo5/youtube/youtubeplaylistdelegate.cpp \
        src/maemo5/youtube/youtubeplaylistdialog.cpp \
        src/maemo5/youtube/youtubeplaylistswindow.cpp \
        src/maemo5/youtube/youtubeplaylistwindow.cpp \
        src/maemo5/youtube/youtubesearchdialog.cpp \
        src/maemo5/youtube/youtubeuserdelegate.cpp \
        src/maemo5/youtube/youtubeuserswindow.cpp \
        src/maemo5/youtube/youtubeuserwindow.cpp \
        src/maemo5/youtube/youtubevideodelegate.cpp \
        src/maemo5/youtube/youtubevideoswindow.cpp \
        src/maemo5/youtube/youtubevideowindow.cpp \
        src/maemo5/youtube/youtubeview.cpp
        
    desktop.files = desktop/maemo5/cutetube2.desktop
    desktop.path = /usr/share/applications/hildon
    
    icon.files = desktop/maemo5/64/cutetube2.png
    icon.path = /usr/share/icons/hicolor/64x64/apps
    
    INSTALLS += desktop icon
    
} else:contains(MEEGO_EDITION,harmattan) {
    DEFINES += NAV_SEARCH

    LIBS += -L../../qdailymotion/lib -lqdailymotion
    LIBS += -L../../qvimeo/lib -lqvimeo
    LIBS += -L../../qyoutube/lib -lqyoutube
    
    QT += declarative opengl
    CONFIG += \
        link_pkgconfig \
        qdeclarative-boostable \
        libtuiclient \
        libcontentaction \
        shareuiinterface-maemo-meegotouch \
        share-ui-common \
        mdatauri \
        qmsystem2
    
    INCLUDEPATH += \
        src/harmattan
    
    HEADERS += \
        src/harmattan/activecolormodel.h \
        src/harmattan/cookiejar.h \
        src/harmattan/maskeditem.h \
        src/harmattan/maskeffect.h \
        src/harmattan/networkaccessmanagerfactory.h \
        src/harmattan/screenorientationmodel.h \
        src/harmattan/screensaver.h \
        src/harmattan/shareui.h
        
    SOURCES += \
        src/harmattan/cookiejar.cpp \
        src/harmattan/main.cpp \
        src/harmattan/maskeditem.cpp \
        src/harmattan/maskeffect.cpp \
        src/harmattan/networkaccessmanagerfactory.cpp \
        src/harmattan/screensaver.cpp \
        src/harmattan/shareui.cpp

    base_qml.files = $$files(src/harmattan/qml/*.qml)
    base_qml.path = /opt/cutetube2/qml

    dailymotion_qml.files = $$files(src/harmattan/qml/dailymotion/*.qml)
    dailymotion_qml.path = /opt/cutetube2/qml/dailymotion

    plugins_qml.files = $$files(src/harmattan/qml/plugins/*.qml)
    plugins_qml.path = /opt/cutetube2/qml/plugins

    vimeo_qml.files = $$files(src/harmattan/qml/vimeo/*.qml)
    vimeo_qml.path = /opt/cutetube2/qml/vimeo

    youtube_qml.files = $$files(src/harmattan/qml/youtube/*.qml)
    youtube_qml.path = /opt/cutetube2/qml/youtube

    images.files = src/harmattan/qml/images/*.png
    images.path = /opt/cutetube2/qml/images

    desktop.files = desktop/harmattan/cutetube2.desktop
    desktop.path = /usr/share/applications

    icon.files = desktop/harmattan/80/cutetube2.png
    icon.path = /usr/share/icons/hicolor/80x80/apps

    contentaction.files = desktop/harmattan/cutetube2.xml
    contentaction.path = /usr/share/contentaction

    splash.files = desktop/harmattan/splash/*.png
    splash.path = /opt/cutetube2/splash

    INSTALLS += \
        base_qml \
        dailymotion_qml \
        plugins_qml \
        vimeo_qml \
        youtube_qml \
        images \
        desktop \
        icon \
        contentaction \
        splash
    
}

unix {
    QT += dbus
    
    INCLUDEPATH += src/dbus
    
    HEADERS += \
        src/base/clipboard.h \
        src/base/audioconverter.h \
        src/base/videoplayermodel.h \
        src/dbus/dbusservice.h
        
    SOURCES += \
        src/base/audioconverter.cpp \
        src/base/clipboard.cpp \
        src/dbus/dbusservice.cpp
    
    dbus_service.files = dbus/org.marxoft.cutetube2.service
    dbus_service.path = /usr/share/dbus-1/services

    dbus_interface.files = dbus/org.marxoft.cutetube2.xml
    dbus_interface.path = /usr/share/dbus-1/interfaces

    target.path = /opt/cutetube2/bin
    
    INSTALLS += dbus_service dbus_interface
}

INSTALLS += target
