#-------------------------------------------------
#
# Project created by QtCreator 2013-06-25T10:17:36
#
#-------------------------------------------------

QT       -= gui

TARGET = BGMRPCInterface
TEMPLATE = lib

DEFINES += BGMRPCINTERFACE_LIBRARY

SOURCES += bgmrpcinterface.cpp \
    datatype.cpp

HEADERS += bgmrpcinterface.h\
        bgmrpcinterface_global.h \
    datatype.h

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}

SHDROOT=../..
include (../../base.pri)
