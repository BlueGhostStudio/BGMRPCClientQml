#include "objctrlcmdcontroller.h"

ObjCtrlCmdController::ObjCtrlCmdController(ObjectInterface* objIF,
                                           QObject* parent)
    : QObject{ parent }, m_objIF{ objIF } {
    QObject::connect(this, &ObjCtrlCmdController::ctrlCmdOperate, m_objIF,
                     &ObjectInterface::objCtrlCmdWork);
    QObject::connect(m_objIF, &ObjectInterface::objCtrlCmdReady, this, [&](const QByteArray& data) {
        m_result = data;
        m_eventLoop.exit();
        deleteLater();
    });
}

QByteArray
ObjCtrlCmdController::ctrlCmd(quint8 cmd, const QByteArray& args) {
    emit ctrlCmdOperate(cmd, args);

    m_eventLoop.exec();

    return m_result;
}
