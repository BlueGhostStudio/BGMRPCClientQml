#ifndef CTRLBASE_H
#define CTRLBASE_H

#include <bgmrpccommon.h>

#include <QLocalSocket>
#include <QObject>

#include "bgmrpc.h"

namespace NS_BGMRPC {

class CtrlBase : public QObject {
    Q_OBJECT
public:
    explicit CtrlBase(BGMRPC* bgmrpc, QLocalSocket* socket,
                      QObject* parent = nullptr);

    void sendCtrlData(const QByteArray& data);
    void closeCtrlSocket();

signals:

protected:
    virtual bool ctrl(const QByteArray& data);

protected:
    BGMRPC* m_BGMRPC;
    QLocalSocket* m_ctrlSocket;
};

}  // namespace NS_BGMRPC

#endif  // CTRLBASE_H
