TEMPLATE = app
TARGET = cutetube2-rss
QT += network xml
QT -= gui

HEADERS += \
    src/json.h \
    src/rss.h
    
SOURCES += \
    src/json.cpp \
    src/main.cpp \
    src/rss.cpp

symbian {    
    plugin.files = plugin/symbian/rss.plugin
    plugin.path = !:/.config/cuteTube2/plugins
    
    settings.files = settings/rss.settings
    settings.path = !:/.config/cuteTube2/plugins/rss
} else:unix {    
    plugin.files = plugin/linux/rss.plugin
    plugin.path = /opt/cutetube2/plugins
    
    settings.files = settings/rss.settings
    settings.path = /opt/cutetube2/plugins/rss
    
    target.path = /opt/cutetube2/plugins/rss
    
    INSTALLS += plugin settings target
}
    