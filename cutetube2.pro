TEMPLATE = subdirs
SUBDIRS += \
    app

contains(MEEGO_EDITION,harmattan) {
    SUBDIRS += \
        ../qdailymotion/src \
        ../qvimeo/src \
        ../qyoutube/src

    app.depends += \
        ../qdailymotion/src \
        ../qvimeo/src \
        ../qyoutube/src
}
