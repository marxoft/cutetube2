QT += core network
QT -= gui
CONFIG += plugin
TARGET = cutetube2-videoclip
TEMPLATE = lib

HEADERS += \
    json.h \
    videoclipplugin.h \
    videocliprequest.h

SOURCES += \
    json.cpp \
    videocliprequest.cpp

symbian {
    TARGET.UID3 = 0xA32FA24B
    TARGET.CAPABILITY += NetworkServices ReadUserData WriteUserData
    TARGET.EPOCALLOWDLLDATA = 1
    TARGET.EPOCHEAPSIZE = 0x20000 0x8000000
    TARGET.EPOCSTACKSIZE = 0x14000

    DEFINES += QYOUTUBE_STATIC_LIBRARY
    QT += script

    INCLUDEPATH += \
        ../src \
        ../../../qyoutube/src

    HEADERS += \
        ../src/resourcesrequest.h \
        ../src/serviceplugin.h \
        ../../../qyoutube/src/qyoutube_global.h \
        ../../../qyoutube/src/request.h \
        ../../../qyoutube/src/request_p.h \
        ../../../qyoutube/src/streamsrequest.h \
        ../../../qyoutube/src/urls.h

    SOURCES += \
        ../../../qyoutube/src/request.cpp \
        ../../../qyoutube/src/streamsrequest.cpp
    
    vendorinfo += "%{\"Stuart Howarth\"}" ":\"Stuart Howarth\""
    videoclip_deployment.pkg_prerules += vendorinfo

    config.sources = "$$TARGET".json
    config.path = !:/cutetube2/plugins

    lib.sources = "$$TARGET".dll
    lib.path = !:/sys/bin

    stub.sources = "$$TARGET".qtplugin
    stub.path = !:/cutetube2/plugins

    DEPLOYMENT.display_name = cuteTube2 Videoclip.bg
    DEPLOYMENT += \
        videoclip_deployment \
        config \
        lib \
        stub

} else:contains(MEEGO_EDITION,harmattan) {
    DEFINES += QYOUTUBE_STATIC_LIBRARY
    QT += script

    INCLUDEPATH += \
        ../src \
        ../../../qyoutube/src

    HEADERS += \
        ../src/resourcesrequest.h \
        ../src/serviceplugin.h \
        ../../../qyoutube/src/qyoutube_global.h \
        ../../../qyoutube/src/request.h \
        ../../../qyoutube/src/request_p.h \
        ../../../qyoutube/src/streamsrequest.h \
        ../../../qyoutube/src/urls.h

    SOURCES += \
        ../../../qyoutube/src/request.cpp \
        ../../../qyoutube/src/streamsrequest.cpp
    
    config.files = "$$TARGET".json
    config.path = /opt/cutetube2/plugins
    
    contentaction.files = "$$TARGET".xml
    contentaction.path = /usr/share/contentaction

    target.path = /opt/cutetube2/plugins
    
    INSTALLS += \
        target \
        config \
        contentaction

} else:unix {
    LIBS += -L/usr/lib -lqyoutube
    CONFIG += link_prl
    PKGCONFIG += libqyoutube
    
    INCLUDEPATH += /usr/include/cutetube2
    HEADERS += \
        /usr/include/cutetube2/resourcesrequest.h \
        /usr/include/cutetube2/serviceplugin.h
    
    config.files = "$$TARGET".json
    config.path = /opt/cutetube2/plugins

    target.path = /opt/cutetube2/plugins
    
    INSTALLS += \
        target \
        config
}
