#-------------------------------------------------
#
# Project created by QtCreator 2013-09-21T14:16:28
#
#-------------------------------------------------

QT += script sql
TARGET = jsPlugin

DEFINES += JSPLUGIN_LIBRARY

SOURCES += jsplugin.cpp \
#    jsproc.cpp \
    jsmetatype.cpp \
#    jsprocclass.cpp \
#    jsglobalobject.cpp \
    jsobjectclass.cpp \
    jsobjectprototype.cpp

HEADERS += jsplugin.h\
        jsplugin_global.h \
#    jsproc.h \
    jsmetatype.h \
#    jsprocclass.h \
#    jsglobalobject.h \
    jsobjectclass.h \
    jsobjectprototype.h

SERLIBPATH = ../BGMRPC
include (../plugin.pri)
