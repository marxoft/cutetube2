TEMPLATE = app
TARGET = cutetube2-localvideos
QT -= gui

#DEFINES += CUTETUBE_DEBUG

greaterThan(QT_MAJOR_VERSION,4) {
    QT += docgallery
} else:maemo5 {
    CONFIG += mobility12
    MOBILITY += gallery
} else: {
    CONFIG += mobility
    MOBILITY += gallery
}

MOBILITY += gallery

HEADERS += \
    src/galleryrequest.h \
    src/json.h

SOURCES += \
    src/galleryrequest.cpp \
    src/json.cpp \
    src/main.cpp
    
symbian {    
    plugin.files = plugin/symbian/localvideos.plugin
    plugin.path = !:/.config/cuteTube2/plugins
    
    settings.files = settings/localvideos.settings
    settings.path = !:/.config/cuteTube2/plugins/localvideos
} else:unix {    
    plugin.files = plugin/linux/localvideos.plugin
    plugin.path = /opt/cutetube2/plugins
    
    settings.files = settings/localvideos.settings
    settings.path = /opt/cutetube2/plugins/localvideos
    
    target.path = /opt/cutetube2/plugins/localvideos
    
    INSTALLS += plugin settings target
}
