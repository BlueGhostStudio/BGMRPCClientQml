unix:!macx: LIBS += -L$$OUT_PWD/../JsModuleCommon/ -lJsModuleCommon

INCLUDEPATH += $$PWD/JsModuleCommon
DEPENDPATH += $$PWD/JsModuleCommon

unix:!macx: PRE_TARGETDEPS += $$OUT_PWD/../JsModuleCommon/libJsModuleCommon.a

!isEmpty (PREFIX) {
    target.path = $${PREFIX}/BGMRPC/JsModules
    target.files = $${OUT_PWD}/*.so
    INSTALLS += target
}

QT       += qml
