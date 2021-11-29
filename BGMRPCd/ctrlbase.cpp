#include "ctrlbase.h"

using namespace NS_BGMRPC;

CtrlBase::CtrlBase(BGMRPC* bgmrpc, QLocalSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_ctrlSocket(socket) {
    QObject::connect(m_ctrlSocket, &QLocalSocket::readyRead,
                     [&]() { ctrl(m_ctrlSocket->readAll()); });

    QObject::connect(m_ctrlSocket, &QLocalSocket::disconnected, [&]() {
        m_ctrlSocket->deleteLater();
        deleteLater();
    });
}

void
CtrlBase::sendCtrlData(const QByteArray& data) {
    m_ctrlSocket->write(data);
}

void
CtrlBase::closeCtrlSocket() {
    m_ctrlSocket->disconnectFromServer();
}

bool
CtrlBase::ctrl(const QByteArray& data) {
    // FINISHED 完成CtrlBase::ctrl
    bool ok = true;

    switch (data[0]) {
    case NS_BGMRPC::CTRL_CONFIG:
        sendCtrlData(m_BGMRPC->getConfig(data[1]));
        break;
    case NS_BGMRPC::CTRL_SETTING:
        sendCtrlData(m_BGMRPC->getSetting(data.mid(1)));
        break;
    case NS_BGMRPC::CTRL_CHECKOBJECT:
        if (m_BGMRPC->checkObject(data.mid(1)))
            sendCtrlData(QByteArray(1, '\x1'));
        else
            sendCtrlData(QByteArray(1, '\x0'));
        break;
    default:
        ok = false;
    }

    return ok;
}
