#-------------------------------------------------
#
# Project created by QtCreator 2013-09-21T14:16:28
#
#-------------------------------------------------

QT += script sql
TARGET = jsPlugin

DEFINES += JSPLUGIN_LIBRARY

SOURCES += jsplugin.cpp \
    jsmetatype.cpp \
    jsobjectclass.cpp \
    jsobjectprototype.cpp

HEADERS += jsplugin.h\
        jsplugin_global.h \
    jsmetatype.h \
    jsobjectclass.h \
    jsobjectprototype.h \
    jsmetatypecommon.h

AUTOLOAD = /autoload
SERLIBPATH = ../BGMRPC
include (../plugin.pri)
