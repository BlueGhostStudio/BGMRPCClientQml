#-------------------------------------------------
#
# Project created by QtCreator 2013-06-24T07:06:19
#
#-------------------------------------------------

QT       += network core script

QT       -= gui

TARGET = clientDemo
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

CLIINTERPATH=../BGMRPCInterface
include (../client.pri)

