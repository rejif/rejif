#src

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

qtHaveModule(printsupport): QT += printsupport
win32:QT += winextras

TARGET = rejif
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/mwindow.h \
    $$PWD/slib.h \
    $$PWD/widget/info.h \
    $$PWD/widget/preview.h \
    $$PWD/core/image.h \
    $$PWD/core/scene.h \
    $$PWD/widget/monitor.h

SOURCES += \
    $$PWD/main.cpp

RESOURCES += \
    $$PWD/resources/resource.qrc

DISTFILES += \
    $$PWD/icon.rc

win32 {
    RC_FILE += $$PWD/icon.rc
}


