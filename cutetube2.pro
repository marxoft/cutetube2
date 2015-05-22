TEMPLATE = subdirs
SUBDIRS += \
    app

contains(MEEGO_EDITION,harmattan) {
    SUBDIRS += \
        ../qdailymotion \
        ../qvimeo \
        ../qyoutube

    app.depends += \
        ../qdailymotion \
        ../qvimeo \
        ../qyoutube

    OTHER_FILES += \
        qtc_packaging/debian_harmattan/rules \
        qtc_packaging/debian_harmattan/manifest.aegis \
        qtc_packaging/debian_harmattan/control \
        qtc_packaging/debian_harmattan/compat \
        qtc_packaging/debian_harmattan/changelog
}
