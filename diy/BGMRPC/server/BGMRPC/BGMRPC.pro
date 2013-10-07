#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T10:40:28
#
#-------------------------------------------------

QT       += network core

QT       -= gui

TARGET = BGMRPC
TEMPLATE = lib

DEFINES += BGMRPC_LIBRARY

SOURCES += \
    bgmrtcpserver.cpp \
    bgmrpc.cpp \
    bgmrobjectstorage.cpp \
    bgmrprocedure.cpp \
    relatedproc.cpp

HEADERS +=\
    bgmrobject.h \
    bgmradaptor.h \
    bgmrpc.h \
    bgmrpc_global.h \
    bgmrtcpserver.h \
    bgmrobjectstorage.h \
    bgmrprocedure.h \
    relatedproc.h

unix {
    target.path = $${PREFIX}/lib
    INSTALLS += target
}

include (../../base.pri)
