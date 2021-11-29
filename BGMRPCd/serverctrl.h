#ifndef SERVERCTRL_H
#define SERVERCTRL_H

#include <bgmrpccommon.h>

#include <QLocalSocket>
#include <QObject>

#include "ctrlbase.h"

namespace NS_BGMRPC {

class ServerCtrl : public CtrlBase {
    Q_OBJECT
public:
    explicit ServerCtrl(BGMRPC* bgmrpc, QLocalSocket* socket,
                        QObject* parent = nullptr);

signals:

protected:
    bool ctrl(const QByteArray& data) override;

private:
};

}  // namespace NS_BGMRPC

#endif  // SERVERCTRL_H
