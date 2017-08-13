#include "bgmrtcpserver.h"
#include "bgmrclient.h"
#include "bgmrpc.h"
#include "bgmrobjectstorage.h"
#include "bgmrobject.h"

namespace BGMircroRPCServer {

BGMRTcpServer::BGMRTcpServer(BGMRPC*r, QObject *parent)
    : __server(QString (), QWebSocketServer::NonSecureMode, parent), RPC (r),Port (8000)
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
                 this, SLOT(newClient()));
    else
        qDebug () << "error" << errorString ();

    return ok;
}

void BGMRTcpServer::setPort(quint16 port)
{
    Port = port;
}

void BGMRTcpServer::newClient()
{
    __socket* clientSocket = nextPendingConnection ();

    qDebug () << QObject::tr ("New client connected.");
    new BGMRClient (RPC, clientSocket);
}

}
