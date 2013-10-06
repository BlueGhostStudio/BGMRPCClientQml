#-------------------------------------------------
#
# Project created by QtCreator 2013-07-25T06:55:50
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = BGMRCall
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

CLIINTERPATH=../BGMRPCInterface
include (../client.pri)

HEADERS += \
    ../global/common.h
