#include <QDataStream>

#include "bgmrpc.h"
#include "client.h"
#include "objectctrl.h"
#include <bgmrpccommon.h>

using namespace NS_BGMRPC;

ObjectCtrl::ObjectCtrl(BGMRPC* bgmrpc, QLocalSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_ctrlStroke(socket)
{
    QObject::connect(m_ctrlStroke, &QLocalSocket::readyRead, [&]() {
        QByteArray data = m_ctrlStroke->readAll();
        switch (data[0]) {
        case NS_BGMRPC::CTRL_REGISTER:
            m_objectName = data.mid(1);
            m_dataSocketName = /*NS_BGMRPC::*/ BGMRPCObjPrefix + m_objectName;
            registerObject(m_objectName);
            break;
        case NS_BGMRPC::CTRL_CHECKOBJECT:
            if (m_BGMRPC->m_objects.contains(data.mid(1)))
                m_ctrlStroke->write("\x1", 1);
            else
                m_ctrlStroke->write("\x0", 1);
            break;
        }
    });
    QObject::connect(m_ctrlStroke, &QLocalSocket::disconnected, [&]() {
        //        qDebug() << "obj name:" << m_objectName << "disconnected";
        removeObject(m_objectName);
    });
}

QString ObjectCtrl::dataSocketName() const
{
    return m_dataSocketName;
}
