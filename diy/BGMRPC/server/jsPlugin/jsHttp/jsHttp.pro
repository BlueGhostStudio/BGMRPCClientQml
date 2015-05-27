#-------------------------------------------------
#
# Project created by QtCreator 2015-04-23T06:32:37
#
#-------------------------------------------------

QT       -= gui
QT       += script\
            network

TARGET = jsHttp
TEMPLATE = lib

DEFINES += JSHTTP_LIBRARY

SOURCES += jshttpproto.cpp \
    loader.cpp \
    jsmetatype.cpp

HEADERS += jshttpproto.h\
        jshttp_global.h \
    loader.h \
    jsmetatype.h

INCLUDEPATH += ../

!isEmpty (PREFIX) {
    target.path = $${PREFIX}/BGMRPC/modules/JS
    target.files = $${OUT_PWD}/*.so
    INSTALLS += target
}

SERLIBPATH = ../../BGMRPC
COMMONPATH=../../../BGMRPCCommon
include (../../server.pri)
