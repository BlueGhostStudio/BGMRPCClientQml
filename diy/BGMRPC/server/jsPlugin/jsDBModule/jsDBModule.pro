#-------------------------------------------------
#
# Project created by QtCreator 2015-01-20T16:44:08
#
#-------------------------------------------------

QT       += sql script

QT       -= core gui

TARGET = jsDBModule
TEMPLATE = lib

DEFINES += JSDBMODULE_LIBRARY

SOURCES += \
    jsdbmproto.cpp \
    jsmetatype.cpp \
    loader.cpp

HEADERS +=\
    jsdbmproto.h \
    jsmetatype.h \
    loader.h

SERLIBPATH = ../../BGMRPC
COMMONPATH=../../../BGMRPCCommon

INCLUDEPATH += ../\
                ../../BGMRPCDBModule/
LIBS += -l$$OUT_PWD/../../BGMRPCDBModule/ -lBGMRPCDBModule

!isEmpty (PREFIX) {
    target.path = $${PREFIX}/BGMRPC/modules/JS
    target.files = $${OUT_PWD}/*.so
    INSTALLS += target
}

include (../../server.pri)
