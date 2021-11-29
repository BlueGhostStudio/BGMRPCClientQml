#include "serverctrl.h"

using namespace NS_BGMRPC;

ServerCtrl::ServerCtrl(BGMRPC* bgmrpc, QLocalSocket* socket, QObject* parent)
    : CtrlBase(bgmrpc, socket, parent) {}

bool
ServerCtrl::ctrl(const QByteArray& data) {
    if (!CtrlBase::ctrl(data)) {
        switch (data[0]) {
        case NS_BGMRPC::CTRL_STOPSERVER:
            m_BGMRPC->stopServer();
            break;
        case NS_BGMRPC::CTRL_DETACHOBJECT:
            /*if (m_BGMRPC->removeObject(data.mid(1)))
                sendCtrlData(QByteArray('\x1', 1));
            else
                sendCtrlData(QByteArray('\x0', 1));*/
            if (m_BGMRPC->detachObject(data.mid(1)))
                sendCtrlData(QByteArray(1, '\x1'));
            else
                sendCtrlData(QByteArray(1, '\x0'));
            break;
        case NS_BGMRPC::CTRL_LISTOBJECTS: {
            QByteArray listData = m_BGMRPC->listObjects();
            if (listData.length() > 0)
                sendCtrlData(listData);
            else
                sendCtrlData(QByteArray(1, '\x0'));
        } break;
        case NS_BGMRPC::CTRL_CHECKOBJECT:
            if (m_BGMRPC->checkObject(data.mid(1)))
                sendCtrlData(QByteArray(1, '\x1'));
            else
                sendCtrlData(QByteArray(1, '\x0'));
            break;
        }
    }

    return true;
}
