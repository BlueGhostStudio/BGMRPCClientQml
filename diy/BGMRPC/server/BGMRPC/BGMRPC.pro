#-------------------------------------------------
#
# Project created by QtCreator 2013-06-17T10:40:28
#
#-------------------------------------------------

QT       += network core websockets

QT       -= gui

TARGET = BGMRPC
TEMPLATE = lib

DEFINES += BGMRPC_LIBRARY

SOURCES += \
    bgmrtcpserver.cpp \
    bgmrpc.cpp \
    bgmrobjectstorage.cpp \
    bgmrclient.cpp \
    relatedclient.cpp \
    bgmrobject.cpp

HEADERS +=\
    bgmrobject.h \
    bgmradaptor.h \
    bgmrpc.h \
    bgmrpc_global.h \
    bgmrtcpserver.h \
    bgmrobjectstorage.h \
    bgmrclient.h \
    relatedclient.h

unix {
    target.path = $${PREFIX}/lib
    INSTALLS += target
}

COMMONPATH=../../BGMRPCCommon
include (../../base.pri)
