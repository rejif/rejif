
QT += widgets
qtHaveModule(printsupport): QT += printsupport

HEADERS       = $$PWD/mainwindow.h \
                $$PWD/scribblearea.h
SOURCES       = $$PWD/main.cpp \
                $$PWD/mainwindow.cpp \
                $$PWD/scribblearea.cpp
