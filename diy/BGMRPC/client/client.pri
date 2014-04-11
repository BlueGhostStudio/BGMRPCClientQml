win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$CLIINTERPATH/release/ -lBGMRPCInterface
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$CLIINTERPATH/debug/ -lBGMRPCInterface
else:unix: LIBS += -L$$OUT_PWD/$$CLIINTERPATH/ -lBGMRPCInterface

INCLUDEPATH += \
    $$PWD/BGMRPCInterface \
    $$PWD/global
DEPENDPATH += $$PWD/BGMRPCInterface

!isEmpty (PREFIX) {
    target.path = $${PREFIX}/BGMRPC/cli
    INSTALLS += target
}
COMMONPATH=../../BGMRPCCommon
include (../base.pri)
