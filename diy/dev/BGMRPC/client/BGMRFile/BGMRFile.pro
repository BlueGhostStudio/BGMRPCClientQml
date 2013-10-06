#-------------------------------------------------
#
# Project created by QtCreator 2013-08-06T22:20:50
#
#-------------------------------------------------

QT       += core network

QT       -= gui

TARGET = BGMRFile
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

CLIINTERPATH=../BGMRPCInterface
include (../client.pri)
