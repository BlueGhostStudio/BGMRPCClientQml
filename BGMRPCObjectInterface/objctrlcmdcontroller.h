#ifndef OBJCTRLCMDCONTROLLER_H
#define OBJCTRLCMDCONTROLLER_H

#include <QEventLoop>
#include <QObject>

#include "objectinterface.h"

using namespace NS_BGMRPCObjectInterface;
class ObjCtrlCmdController : public QObject {
    Q_OBJECT
public:
    explicit ObjCtrlCmdController(ObjectInterface* objIF,
                                  QObject* parent = nullptr);

    QByteArray ctrlCmd(quint8 cmd, const QByteArray& args);

signals:
    void ctrlCmdOperate(quint8 cmd, const QByteArray& arg);

private:
    ObjectInterface* m_objIF;
    QEventLoop m_eventLoop;
    QByteArray m_result;
};

#endif  // OBJCTRLCMDCONTROLLER_H
