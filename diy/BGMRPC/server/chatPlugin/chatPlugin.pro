#-------------------------------------------------
#
# Project created by QtCreator 2013-09-09T09:10:19
#
#-------------------------------------------------

QT += core
TARGET = chatPlugin

DEFINES += CHATPLUGIN_LIBRARY

SOURCES += chatplugin.cpp

HEADERS += chatplugin.h\
        chatplugin_global.h

SERLIBPATH = ../BGMRPC
include (../plugin.pri)
