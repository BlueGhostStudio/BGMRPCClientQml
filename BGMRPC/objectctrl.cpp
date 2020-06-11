#include <QDataStream>

#include "bgmrpc.h"
#include "client.h"
#include "objectctrl.h"
#include <bgmrpccommon.h>

using namespace NS_BGMRPC;

ObjectCtrl::ObjectCtrl(/*BGMRPC* bgmrpc, */ QLocalSocket* socket,
                       QObject* parent)
    : QObject(parent), /*m_BGMRPC(bgmrpc), */ m_ctrlStroke(socket)
{
    m_daemonCtrl = false;
    QObject::connect(m_ctrlStroke, &QLocalSocket::readyRead, [&]() {
        QByteArray data = m_ctrlStroke->readAll();
        switch (data[0]) {
        case NS_BGMRPC::CTRL_REGISTER:
            if (!m_daemonCtrl) {
                m_objectName = data.mid(1);
                m_dataSocketName = BGMRPCObjPrefix + m_objectName;
                registerObject(m_objectName);
            }
            break;
        case NS_BGMRPC::CTRL_CHECKOBJECT:
            checkObject(data.mid(1));
            break;
        case NS_BGMRPC::CTRL_DAEMONCTRL:
            if (m_objectName.isEmpty()) {
                m_daemonCtrl = true;
                m_ctrlStroke->write(QByteArray::fromRawData("\x1", 1));
            } else
                m_ctrlStroke->write(QByteArray::fromRawData("\x0", 1));
            break;
        case NS_BGMRPC::CTRL_STOPSERVER:
            if (m_daemonCtrl)
                stopServer();
            break;
        case NS_BGMRPC::CTRL_CONFIG:
            getConfig(data[1]);
            break;
        case NS_BGMRPC::CTRL_DETACHOBJECT:
            if (m_daemonCtrl)
                detachObject(data.mid(1));
            break;
        case NS_BGMRPC::CTRL_LISTOBJECTS:
            if (m_daemonCtrl)
                listObjects();
        }
    });
    QObject::connect(m_ctrlStroke, &QLocalSocket::disconnected, [&]() {
        m_ctrlStroke->deleteLater();
        deleteLater();
        //        qDebug() << "obj name:" << m_objectName << "disconnected";
        if (!m_daemonCtrl && !m_objectName.isEmpty())
            removeObject(m_objectName);
    });
}

QString ObjectCtrl::dataSocketName() const
{
    return m_dataSocketName;
}

void ObjectCtrl::sendCtrlData(const QByteArray& data)
{
    m_ctrlStroke->write(data);
}
