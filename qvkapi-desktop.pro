#-------------------------------------------------
#
# Project created by QtCreator 2016-03-22T22:10:33
#
#-------------------------------------------------

QT          += core gui network webkitwidgets sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = qvkapi-desktop
TEMPLATE    = app

OBJECTS_DIR = build/obj
RCC_DIR     = build/rcc
MOC_DIR     = build/moc
UI_DIR      = build/ui

INCLUDEPATH += libqvkapi/include data dialogs pages widgets

DEFINES += QVKAPI_TEST

SOURCES += \
    libqvkapi/src/qvkauth.cpp \
    libqvkapi/src/qvkauthview.cpp \
    libqvkapi/src/qvkrequest.cpp \
    libqvkapi/src/qvkrequestalbums.cpp \
    libqvkapi/src/qvkrequestphotos.cpp \
    libqvkapi/src/qvkrequestusers.cpp \
    data/accountinfo.cpp \
    data/imagecache.cpp \
    data/metadata.cpp \
    data/settings.cpp \
    dialogs/dialogautent.cpp \
    dialogs/dialogauthorize.cpp \
    dialogs/dialogsettings.cpp \
    main.cpp\
    mainwindow.cpp \
    pages/vkpagealbums.cpp \
    pages/vkpagephotos.cpp \
    pages/vkpageuser.cpp \
    pages/vkpagewidget.cpp \
    widgets/vkimagewidget.cpp \
    widgets/vkthumbnail.cpp

HEADERS  += \
    libqvkapi/include/qvkalbuminfo.h \
    libqvkapi/include/qvkapi_global.h \
    libqvkapi/include/qvkauth.h \
    libqvkapi/include/qvkauthview.h \
    libqvkapi/include/qvkinfophoto.h \
    libqvkapi/include/qvkrequest.h \
    libqvkapi/include/qvkrequestalbums.h \
    libqvkapi/include/qvkrequestphotos.h \
    libqvkapi/include/qvkrequestusers.h \
    libqvkapi/include/qvkuserinfo.h \
    data/accountinfo.h \
    data/imagecache.h \
    data/metadata.h \
    data/resicons.h \
    data/settings.h \
    dialogs/dialogautent.h \
    dialogs/dialogauthorize.h \
    dialogs/dialogsettings.h \
    mainwindow.h \
    pages/vkpagealbums.h \
    pages/vkpagephotos.h \
    pages/vkpageuser.h \
    pages/vkpagewidget.h \
    widgets/vkimagewidget.h \
    widgets/vkthumbnail.h

FORMS    += \
    dialogs/dialogautent.ui \
    dialogs/dialogauthorize.ui \
    dialogs/dialogsettings.ui \
    mainwindow.ui \
    pages/vkpagealbums.ui \
    pages/vkpagephotos.ui \
    pages/vkpageuser.ui

RESOURCES += res/resource.qrc
