#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T06:32:37
#
#-------------------------------------------------

QT       -= gui

TARGET = jsHttp
TEMPLATE = lib

DEFINES += JSHTTP_LIBRARY

SOURCES += jshttpproto.cpp

HEADERS += jshttpproto.h\
        jshttp_global.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}
