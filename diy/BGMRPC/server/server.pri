win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SERLIBPATH/release/ -lBGMRPC
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SERLIBPATH/debug/ -lBGMRPC
else:unix:!macx: LIBS += -L$$OUT_PWD/$$SERLIBPATH/ -lBGMRPC

INCLUDEPATH += $$PWD/BGMRPC
DEPENDPATH += $$PWD/BGMRPC

isEmpty (COMMONPATH) {
    COMMONPATH=../../BGMRPCCommon
}
include (../base.pri)
