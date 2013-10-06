#ifndef SOCKET_H
#define SOCKET_H

#ifdef WEBSOCKET
#include <QWsServer.h>
#include <QWsSocket.h>
typedef QtWebsocket::QWsServer __server;
typedef QtWebsocket::QWsSocket __socket;

#else

#include <QTcpServer>
#include <QTcpSocket>
typedef QTcpServer __server;
typedef QTcpSocket __socket;

#endif
#endif // SOCKET_H
