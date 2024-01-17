#include "ctrlbase.h"

using namespace NS_BGMRPC;

CtrlBase::CtrlBase(BGMRPC* bgmrpc, QLocalSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_ctrlSlot(socket) {
    QObject::connect(m_ctrlSlot, &QLocalSocket::readyRead, this,
                     [&]() { ctrl(m_ctrlSlot->readAll()); });

    QObject::connect(m_ctrlSlot, &QLocalSocket::disconnected, this, [&]() {
        m_ctrlSlot->deleteLater();
        deleteLater();
    });
}

/*void
CtrlBase::sendCtrlData(const QByteArray& data) {
    m_ctrlSlot->write(data);
}*/

void
CtrlBase::closeCtrlSocket() {
    m_ctrlSlot->disconnectFromServer();
}

bool
CtrlBase::ctrl(const QByteArray& data) {
    // FINISHED 完成CtrlBase::ctrl
    bool ok = true;

    switch (data[0]) {
    case NS_BGMRPC::CTRL_CONFIG:
        //sendCtrlData(m_BGMRPC->getConfig(data[1]));
        m_ctrlSlot->write(m_BGMRPC->getConfig(data[1]));
        break;
    case NS_BGMRPC::CTRL_SETTING:
        //sendCtrlData(m_BGMRPC->getSetting(data.mid(1)));
        m_ctrlSlot->write(m_BGMRPC->getSetting(data.mid(1)));
        break;
    case NS_BGMRPC::CTRL_CHECKOBJECT:
        if (m_BGMRPC->checkObject(data.mid(1)))
            m_ctrlSlot->write(QByteArray(1, '\x1'));
            //sendCtrlData(QByteArray(1, '\x1'));
        else
            m_ctrlSlot->write(QByteArray(1, '\x0'));
            //sendCtrlData(QByteArray(1, '\x0'));
        break;
    default:
        ok = false;
    }

    return ok;
}
