#-------------------------------------------------
#
# Project created by QtCreator 2017-09-15T16:51:52
#
#-------------------------------------------------

QT       += network sql

TARGET = BGResourcePlugin

DEFINES += BGRESOURCEPLUGIN_LIBRARY

SOURCES += \
    bgresourceplugin.cpp

HEADERS += \
    bgresourceplugin_global.h \
    bgresourceplugin.h

AUTOLOAD = /autoload
SERLIBPATH=../../BGMRPC
COMMONPATH=../../../BGMRPCCommon
include (../../plugin.pri)



unix:!macx: LIBS += -L$$OUT_PWD/../qhttp/xbin/ -lqhttp

INCLUDEPATH += $$PWD/../qhttp/src
DEPENDPATH += $$PWD/../qhttp/src

qhttpLib.files = $${OUT_PWD}/../qhttp/xbin/libqhttp.*
qhttpLib.path = $${PREFIX}/lib
INSTALLS += qhttpLib
