#-------------------------------------------------
#
# Project created by QtCreator 2013-07-17T15:07:53
#
#-------------------------------------------------

TARGET = RPCCtrlPlugin
DEFINES += RPCCTRLPLUGIN_LIBRARY

SOURCES += \
    rpcctrlplugin.cpp

HEADERS += \
    rpcctrlplugin_global.h \
    rpcctrlplugin.h

SERLIBPATH=../BGMRPC
include (../plugin.pri)
