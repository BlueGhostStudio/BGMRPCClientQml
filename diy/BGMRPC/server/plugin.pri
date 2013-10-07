QT       -= gui
TEMPLATE = lib
unix {
    !isEmpty (PREFIX) {
        target.path = $${PREFIX}/BGMRPC/plugins$${AUTOLOAD}
        target.files = *.so
        INSTALLS += target
    }
}
include (server.pri)
