#src

QT       += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = rejif
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    $$PWD/mwindow.h \
    $$PWD/scribblearea.h \
    $$PWD/slib.h \
    $$PWD/image.h

SOURCES += \
    $$PWD/main.cpp

RESOURCES += \
    $$PWD/resources/resource.qrc
