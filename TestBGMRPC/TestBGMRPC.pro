QT += testlib network websockets
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_clientcall.cpp \
    ../BGMRPC/bgmrpc.cpp \
    ../BGMRPC/client.cpp \
    ../BGMRPC/objectctrl.cpp

HEADERS += \
    ../BGMRPC/bgmrpc.h \
    ../BGMRPC/client.h \
    ../BGMRPC/objectctrl.h

INCLUDEPATH += ../BGMRPC
INCLUDEPATH += ../BGMRPCObjectLoader

unix:!macx: LIBS += -L$$OUT_PWD/../BGMRPCCommon/ -lBGMRPCCommon

INCLUDEPATH += $$PWD/../BGMRPCCommon
DEPENDPATH += $$PWD/../BGMRPCCommon

unix:!macx: LIBS += -L$$OUT_PWD/../BGMRPCObjectInterface/ -lBGMRPCObjectInterface

INCLUDEPATH += $$PWD/../BGMRPCObjectInterface
DEPENDPATH += $$PWD/../BGMRPCObjectInterface

unix:!macx: LIBS += -L$$OUT_PWD/../BGMRPCClient/ -lBGMRPCClient

INCLUDEPATH += $$PWD/../BGMRPCClient
DEPENDPATH += $$PWD/../BGMRPCClient

DISTFILES +=
