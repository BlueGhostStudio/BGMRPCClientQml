#-------------------------------------------------
#
# Project created by QtCreator 2014-12-12T11:37:09
#
#-------------------------------------------------

QT       += script

QT       -= gui

TARGET = jsFile
TEMPLATE = lib

DEFINES += JSFILEMODULE_LIBRARY

SOURCES += \
    jsfileproto.cpp \
    jsmetatype.cpp \
    loader.cpp

HEADERS += \
    jsfileproto.h \
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
