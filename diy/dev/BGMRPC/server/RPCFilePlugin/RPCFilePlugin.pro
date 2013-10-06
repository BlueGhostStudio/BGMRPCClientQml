#-------------------------------------------------
#
# Project created by QtCreator 2013-07-28T16:55:04
#
#-------------------------------------------------

TARGET = RPCFilePlugin

DEFINES += RPCFILEPLUGIN_LIBRARY

SOURCES += rpcfileplugin.cpp \
    filestream.cpp

HEADERS += rpcfileplugin.h\
        rpcfileplugin_global.h \
    filestream.h

SERLIBPATH=../BGMRPC
include (../plugin.pri)
