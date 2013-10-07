#include <QCoreApplication>
#include "bgmrpcinterface.h"
#include <QJsonArray>
#include <QJsonObject>
#include <datatype.h>
#include <QtScript>

#define SERVERIP "127.0.0.1"

using namespace BGMircroRPCInterface;
using namespace BGMircroRPC;

BGMRPCInterface theClient (QHostAddress (SERVERIP), 8000);

void testUnkeepConnect ()
{
    returnedValue_t returnedValues;

    for (int i = 0; i < 100; i++)
        returnedValues
                = theClient.callMethod (
                      "mathDemo", "plus",
                      args_t () << (double)returnedValues << i);

    qDebug () << (double) returnedValues;
}

int testKeepConnect ()
{
    qDebug () << "testKeepConnect";
    qulonglong pID = theClient.callMethod ("ctrl", "connect",
                                           args_t (), true).pID ();
    theClient.callMethod (pID, "use",
                          args_t () << QString ("math1"));
    returnedValue_t returnedValues;

    for (int i = 0; i <= 10; i++)
        returnedValues
                = theClient.callMethod (
                      pID, "plus",
                      args_t () << (double)returnedValues << i);

    qDebug () << (double) returnedValues;

    //theClient.callMethod (pID, "ctrl", "close", args_t());

    return pID;
}

void info ()
{
    QJsonArray objs = theClient.callMethod ("ctrl", "RPCObjects", args_t());
    for (int i = 0; i < objs.count (); i++) {
        qDebug () << objs[i].toObject ()["name"].toString ();
        qDebug () << objs[i].toObject ()["type"].toString ();
    }

    QJsonArray methods = theClient.callMethod ("ctrl", "objectMethods",
                                               args_t () << QString ("mathDemo"));
    for (int i = 0; i < methods.count (); i++)
        qDebug () << methods[i].toString ();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    __socket tcpSocket;
    tcpSocket.connectToHost (QHostAddress ("174.129.147.81"), 8000);
    if (tcpSocket.waitForConnected ()) {
        QByteArray headPart (" / HTTP/1.1\n"
                             "Connection: Upgrade\n"
                             "Upgrade: websocket\n"
                             "Sec-WebSocket-Version: 13\n"
                             "Sec-WebSocket-Key: MTMtMTM3NzU4Mjk1MjgzNA==\n"
                             "Sec-WebSocket-Protocol: chat\n"
                             "Host: dev-bgmrpc.rhcloud.com\n\n");
        tcpSocket.write (
                    "GET" + headPart);
        if (tcpSocket.waitForReadyRead ())
            qDebug () << tcpSocket.readAll ();
        //tcpSocket.write ("\x0This is a test\xff");
        if (tcpSocket.waitForReadyRead ())
            qDebug () << tcpSocket.readAll ();
    }

    //QTimer::singleShot(10, &a, SLOT(quit()));

    return a.exec();
}
