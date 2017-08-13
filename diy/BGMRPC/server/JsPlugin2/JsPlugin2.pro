#-------------------------------------------------
#
# Project created by QtCreator 2017-08-03T13:40:45
#
#-------------------------------------------------

QT       += qml

QT       -= gui

TARGET = JsPlugin2
TEMPLATE = lib

DEFINES += JSPLUGIN2_LIBRARY

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    jsobjs.cpp \
    jsengine.cpp

HEADERS += \
        jsplugin2_global.h \   
    jsobjs.h \
    jsengine.h


AUTOLOAD = /autoload
SERLIBPATH = ../BGMRPC
include (../plugin.pri)

#unix:!macx: LIBS += -L$$OUT_PWD/JsPlugin2Common/ -lJsPlugin2Common

#INCLUDEPATH += $$PWD/JsPlugin2Common
#DEPENDPATH += $$PWD/JsPlugin2Common

#unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/JsPlugin2Common/libJsPlugin2Common.a
