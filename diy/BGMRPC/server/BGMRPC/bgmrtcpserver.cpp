#include "bgmrtcpserver.h"
#include "bgmrprocedure.h"

namespace BGMircroRPCServer {

BGMRTcpServer::BGMRTcpServer(BGMRPC*r, QObject *parent)
    : __server(parent), RPC (r),Port (8000)
{
}

void BGMRTcpServer::initial()
{
    /*connect (this, &BGMRTcpServer::newConnection,
             this, &BGMRTcpServer::newCall);*/
}

bool BGMRTcpServer::activeServer(const QHostAddress& address)
{
    bool ok = listen (address, Port);
    if (ok)
        connect (this, SIGNAL(newConnection()),
                 this, SLOT(newCall()));
    else
        qDebug () << "error" << errorString ();

    return ok;
}

void BGMRTcpServer::setPort(quint16 port)
{
    Port = port;
}

void BGMRTcpServer::newCall()
{
    __socket* procSocket = nextPendingConnection ();
    //connect (procSocket, SIGNAL(disconnected()),
    //         procSocket, SLOT(deleteLater()));

    qDebug () << QObject::tr ("New client connected.");
    new BGMRProcedure (RPC, procSocket);
}

}
