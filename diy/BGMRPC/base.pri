QT       += network websockets

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/$$SERLIBPATH/../../BGMRPCCommon/release/ -lBGMRPCCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/$$SERLIBPATH/../../BGMRPCCommon/debug/ -lBGMRPCCommon
else:unix:!macx: LIBS += -L$$OUT_PWD/$$COMMONPATH/ -lBGMRPCCommon

INCLUDEPATH += $$PWD/BGMRPCCommon
DEPENDPATH += $$PWD/BGMRPCCommon


#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../builder/QtWebsocket/release/ -lQtWebsocket
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../builder/QtWebsocket/debug/ -lQtWebsocket
#else:unix:!macx: LIBS += -L$$PWD/../builder/QtWebsocket/ -lQtWebsocket

#message ($$OUT_PWD)

#INCLUDEPATH += $$PWD/../QtWebsocket
#DEPENDPATH += $$PWD/../QtWebsocket

#INCLUDEPATH += $$PWD/BGMRPCCommon
#HEADERS += $$PWD/BGMRPCCommon/socket.h
