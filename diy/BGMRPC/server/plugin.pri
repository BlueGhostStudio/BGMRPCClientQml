QT       -= gui
TEMPLATE = lib
unix {
    !isEmpty (PREFIX) {
        target.path = $${PREFIX}/BGMRPC/plugins$${AUTOLOAD}
        target.files = $${OUT_PWD}/*.so
        INSTALLS += target
    }
}
include (server.pri)
