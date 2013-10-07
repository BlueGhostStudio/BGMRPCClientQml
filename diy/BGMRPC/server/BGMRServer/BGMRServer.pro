#-------------------------------------------------
#
# Project created by QtCreator 2013-07-25T06:40:53
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = BGMRServer
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

unix {
    target.path = $${PREFIX}/bin
    INSTALLS += target
}

SERLIBPATH=../BGMRPC
include (../server.pri)

