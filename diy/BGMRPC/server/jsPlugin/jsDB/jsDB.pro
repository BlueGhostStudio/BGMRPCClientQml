#-------------------------------------------------
#
# Project created by QtCreator 2014-12-13T17:09:53
#
#-------------------------------------------------

QT       += sql script

QT       -= core gui

TARGET = jsDB
TEMPLATE = lib

SOURCES += \
    jsdbproto.cpp \
    jsmetatype.cpp \
    loader.cpp

HEADERS += \
    jsdbproto.h \
    jsmetatype.h \
    loader.h

INCLUDEPATH += ../

!isEmpty (PREFIX) {
    target.path = $${PREFIX}/BGMRPC/modules/JS
    target.files = $${OUT_PWD}/*.so
    INSTALLS += target
}

SERLIBPATH = ../../BGMRPC
COMMONPATH=../../../BGMRPCCommon
include (../../server.pri)
