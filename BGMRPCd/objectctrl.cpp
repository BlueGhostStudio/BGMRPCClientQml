#include "objectctrl.h"

#include <bgmrpccommon.h>

#include <QDataStream>

#include "bgmrpc.h"
#include "client.h"

using namespace NS_BGMRPC;

/*ObjectCtrl::ObjectCtrl(QLocalSocket* socket,
                       QObject* parent)
    : QObject(parent),  m_ctrlStroke(socket) {
    m_daemonCtrl = false;
    QObject::connect(m_ctrlStroke, &QLocalSocket::readyRead, [&]() {
        QByteArray data = m_ctrlStroke->readAll();
        switch (data[0]) {
        case NS_BGMRPC::CTRL_REGISTER:
            if (!m_daemonCtrl) {
                m_objectName = data.mid(1);
                m_dataSocketName = BGMRPCObjPrefix + m_objectName;
                emit registerObject(m_objectName);
            }
            break;
        case NS_BGMRPC::CTRL_CHECKOBJECT:
            emit checkObject(data.mid(1));
            break;
        case NS_BGMRPC::CTRL_DAEMONCTRL:
            if (m_objectName.isEmpty()) {
                m_daemonCtrl = true;
                m_ctrlStroke->write(QByteArray::fromRawData("\x1", 1));
            } else
                m_ctrlStroke->write(QByteArray::fromRawData("\x0", 1));
            break;
        case NS_BGMRPC::CTRL_STOPSERVER:
            if (m_daemonCtrl) emit stopServer();
            break;
        case NS_BGMRPC::CTRL_CONFIG:
            emit getConfig(data[1]);
            break;
        case NS_BGMRPC::CTRL_SETTING:
            emit getSetting(data.mid(1));
            break;
        case NS_BGMRPC::CTRL_DETACHOBJECT:
            if (m_daemonCtrl) emit detachObject(data.mid(1));
            break;
        case NS_BGMRPC::CTRL_LISTOBJECTS:
            if (m_daemonCtrl) emit listObjects();
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

QString
ObjectCtrl::dataSocketName() const {
    return m_dataSocketName;
}

void
ObjectCtrl::sendCtrlData(const QByteArray& data) {
    m_ctrlStroke->write(data);
}*/

ObjectCtrl::ObjectCtrl(BGMRPC* bgmrpc, QLocalSocket* socket, QObject* parent)
    : CtrlBase(bgmrpc, socket, parent) {
    QObject::connect(m_ctrlSlot, &QLocalSocket::disconnected, this, [&]() {
        m_ctrlSlot->deleteLater();
        deleteLater();
        if (!m_objectName.isEmpty()) m_BGMRPC->removeObject(m_objectName);
    });
}

/*QByteArray
ObjectCtrl::dataSocketName() const {
    return m_dataSocketName;
}*/

/*void
ObjectCtrl::closeCtrlSocket() {
    m_ctrlSocket->disconnectFromServer();
}*/

bool
ObjectCtrl::ctrl(const QByteArray& data) {
    if (!CtrlBase::ctrl(data)) {
        switch (data[0]) {
        case NS_BGMRPC::CTRL_REGISTER:
            m_objectName = data.mid(1);
            m_dataSocketName = BGMRPCObjPrefix + m_objectName;
            if (m_BGMRPC->registerObject(
                    this,
                    m_objectName))  // FINISHED register object
                m_ctrlSlot->write(QByteArray(1, '\x1'));
                //sendCtrlData(QByteArray(1, '\x1'));
            else
                m_ctrlSlot->write(QByteArray(1, '\x0'));
                //sendCtrlData(QByteArray(1, '\x0'));
            break;
        }

        return true;
    }

    return true;
}
