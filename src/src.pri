#src

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

win32:QT += winextras

TARGET = rejif
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/mwindow.h \
    $$PWD/slib.h \
    $$PWD/image.h \
    $$PWD/info.h \
    $$PWD/preview.h \
    $$PWD/scene.h \
    $$PWD/director.h

SOURCES += \
    $$PWD/main.cpp

RESOURCES += \
    $$PWD/resources/resource.qrc

win32 {
    RC_FILE += $$PWD/icon.rc
}


