#ifndef BGMRTCPSERVER_H
#define BGMRTCPSERVER_H

//#include <QTcpServer>
#include <socket.h>
#include "bgmrpc_global.h"

namespace BGMircroRPCServer {

class BGMRPC;

class BGMRPCSHARED_EXPORT BGMRTcpServer : public __server/*QTcpServer*/
{
    Q_OBJECT
public:
    explicit BGMRTcpServer (BGMRPC* r, QObject *parent = 0);

    void initial ();
    bool activeServer(const QHostAddress& address = QHostAddress::Any);
    void setPort (quint16 port);

signals:
    
public slots:
    void newProc();

protected:
    BGMRPC* RPC;
    quint16 Port;
};

}
#endif // BGMRTCPSERVER_H
