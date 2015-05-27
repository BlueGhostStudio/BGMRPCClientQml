#include "bgmrpcinterface.h"
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

namespace BGMircroRPCInterface {

cliRawDataTrans::cliRawDataTrans(__socket* socket, QObject* parent)
    : QObject (parent), CliRawSocket (socket)
{
    connect (CliRawSocket, SIGNAL(disconnected()),
             CliRawSocket, SLOT(deleteLater()));
    connect (CliRawSocket, SIGNAL(disconnected()),
             this, SLOT(deleteLater()));
    connect (CliRawSocket, SIGNAL(disconnected()),
             &WaitLoop, SLOT(quit()));
    connect (CliRawSocket, SIGNAL(readyRead()),
             this, SLOT(readData()));
}

bool cliRawDataTrans::sendRawData(const QByteArray& data)
{
    qint64 index = 0;
    if (data.isEmpty ())
        return true;

    bool ok = true;
    while (index < (data.size ()-1)) {
        index += CliRawSocket->write (data.mid (index, SENDDATA_MAXSIZE));
        ok = CliRawSocket->waitForBytesWritten ();
        if (!ok)
            break;
    }

    return ok;
}

QByteArray cliRawDataTrans::getRawData(qint16 len)
{
    if (CliRawSocket->state () != QAbstractSocket::UnconnectedState
            && BuffData.isEmpty ())
        WaitLoop.exec ();

    QByteArray rawData;
    if (len == -1) {
        rawData = BuffData;
        BuffData.clear ();
    } else {
        rawData = BuffData.left (len);
        BuffData.remove (0, len);
    }

    return rawData;
}

void cliRawDataTrans::close()
{
    CliRawSocket->disconnectFromHost ();
}

void cliRawDataTrans::readData()
{
    BuffData.append (CliRawSocket->readAll ());
    if (WaitLoop.isRunning ())
        WaitLoop.exit ();
}

// =================

cliDataTrans::cliDataTrans(QObject* parent)
    : QObject (parent), SID (-1), BufferReady (false),
      WaitDelete (false)//, RawSocket (isRaw)
{
    CliSocket = new __socket;
    connect (CliSocket, SIGNAL(disconnected()),
             this, SLOT(onDisconnected()));
    connect (CliSocket, SIGNAL(readyRead()),
             this, SLOT(readRemoteData()));
}

void cliDataTrans::setSID(qulonglong id)
{
    SID = id;
}

void cliDataTrans::onDisconnected()
{
    emit disconnected (SID);
    WaitDelete = true;
    /*if (!BufferReady)
        CliSocket.deleteLater ();*/
}

BGMircroRPC::returnedValue_t cliDataTrans::getReturnedValues()
{
    if (!BufferReady) {
        QTimer::singleShot (10000, &WaitReadLoop, SLOT(quit()));
        WaitReadLoop.exec ();
    }

    BGMircroRPC::returnedValue_t returnedValues;
    if (BufferReady) {
        BufferReady = false;
        returnedValues = Buffer;
    }

    if (WaitDelete)
        CliSocket->deleteLater ();

    return returnedValues;
}

__socket* cliDataTrans::socket(bool detach)
{
    __socket* theSocket = CliSocket;
    if (detach) {
        theSocket->disconnect ();
        CliSocket = NULL;
    }

    return theSocket;
}

void cliDataTrans::readRemoteData()
{
#ifdef WEBSOCKET
    BGMircroRPC::returnedValue_t data
            = QJsonDocument::fromJson (CliSocket->readAll ()).object ();
#else
    BGMircroRPC::returnedValue_t data
            = QJsonDocument::fromBinaryData (CliSocket->readAll ()).object ();
#endif

    if (data.isSignal ())
        emit remoteSignal (data.object (), data.signal (), data.args ());

    else if (data.isReturned ()) {
        BufferReady = true;
        Buffer = data;
        if (WaitReadLoop.isRunning ())
            WaitReadLoop.exit ();
    }
}

// ======================

BGMRPCInterface::BGMRPCInterface (QObject* parent)
    : QObject (parent)
{
}

BGMRPCInterface::BGMRPCInterface(const QHostAddress& address,
                                 quint16 port, QObject* parent)
    : QObject (parent), ServerAddress (address), ServerPort (port)
{
}

void BGMRPCInterface::setServerAddress(const QHostAddress& address,
                                       quint16 port)
{
    ServerAddress = address;
    ServerPort = port;
}

void BGMRPCInterface::setHost(const QString& host)
{
    Hosts = host;
}

void BGMRPCInterface::setProxy(const QNetworkProxy& proxy)
{
    Proxy = proxy;
}

BGMircroRPC::returnedValue_t
BGMRPCInterface::callMethod(const QString& obj, const QString& method,
                            const QJsonArray& args, bool keepConnected)
{
    QJsonObject callJson;
    callJson ["object"] = obj;
    callJson ["method"] = method;
    callJson ["args"] = args;
    callJson ["keepConnected"] = keepConnected;

    cliDataTrans* theDataTrans = new cliDataTrans;
    __socket* theCliSocket = theDataTrans->socket ();
    connect (theDataTrans, SIGNAL(remoteSignal(QString,QString,QJsonArray)),
             this, SIGNAL(remoteSignal(QString,QString,QJsonArray)));

    BGMircroRPC::returnedValue_t returnedValues;
    if (connectToHost (theCliSocket)) {
#ifdef WEBSOCKET
//        theCliSocket->write (QJsonDocument(callJson).toJson ());
        theCliSocket->sendBinaryMessage (QJsonDocument (callJson).toJson ());
#else
        theCliSocket->write (QJsonDocument(callJson).toBinaryData ());
#endif
        if (theCliSocket->waitForBytesWritten ()) {
            returnedValues = theDataTrans->getReturnedValues ();

            if (keepConnected) {
                qulonglong sID;
                //sID = returnedValues["pID"].toDouble ();
                sID = returnedValues.pID ();
                CliDataTrans [sID] = theDataTrans;
                theDataTrans->setSID (sID);

                connect (theDataTrans, SIGNAL(disconnected(qulonglong)),
                         this, SLOT(removeCliSocket(qulonglong)));
            } else
                connect (theCliSocket, SIGNAL(disconnected()),
                         theDataTrans, SLOT(deleteLater()));
        }
    } else
        qDebug () << "can't connect to server";

    return returnedValues;
}

BGMircroRPC::returnedValue_t
BGMRPCInterface::callMethod(qulonglong id, const QString& method,
                            const QJsonArray& args)
{
    return callMethod (id, QString (), method, args);
}

BGMircroRPC::returnedValue_t
BGMRPCInterface::callMethod(qulonglong id, const QString& object,
                            const QString& method, const QJsonArray& args)
{
    cliDataTrans* theDataTrans;
    BGMircroRPC::returnedValue_t returnedValues;
    if (CliDataTrans.contains (id)
            && (theDataTrans = CliDataTrans [id])) {
        __socket* theCliSocket = theDataTrans->socket ();
        QJsonObject callJson;
        callJson ["method"] = method;
        callJson ["args"] = args;
        if (!object.isEmpty ())
            callJson ["object"] = object;

#ifdef WEBSOCKET
        // theCliSocket->write (QJsonDocument (callJson).toJson ());
        theCliSocket->sendBinaryMessage (QJsonDocument (callJson).toJson ());
#else
        theCliSocket->write (QJsonDocument (callJson).toBinaryData ());
#endif

        if (theCliSocket->waitForBytesWritten ())
            returnedValues = theDataTrans->getReturnedValues ();
    }

    return returnedValues;
}

cliRawDataTrans*
BGMRPCInterface::callMethodRaw(const QString& obj,
                               const QString& method,
                               const QJsonArray& args)
{
    QJsonObject callJson;
    callJson ["object"] = obj;
    callJson ["method"] = method;
    callJson ["args"] = args;
    callJson ["keepConnected"] = true;

    __socket* theCliSocket = new __socket;
    connect (theCliSocket, SIGNAL(disconnected()),
             theCliSocket, SLOT(deleteLater()));

    if (connectToHost (theCliSocket)) {
#ifdef WEBSOCKET
        // theCliSocket->write (QJsonDocument(callJson).toJson ());
        theCliSocket->sendBinaryMessage (QJsonDocument(callJson).toJson ());

#else
        theCliSocket->write (QJsonDocument(callJson).toBinaryData ());
#endif
        // theCliSocket->waitForBytesWritten ();
    } else
        qDebug () << "can't connect to host";

    return new cliRawDataTrans (theCliSocket);
}

cliRawDataTrans*
BGMRPCInterface::callMethodRaw(qulonglong id, const QString& method,
                               const QJsonArray& args)
{
    return callMethodRaw (id, QString (), method, args);
}

cliRawDataTrans*
BGMRPCInterface::callMethodRaw(qulonglong id, const QString& object,
                               const QString& method, const QJsonArray& args)
{
    __socket* theCliSocket = NULL;
    if (CliDataTrans.contains (id)
            && (theCliSocket = CliDataTrans.take (id)->socket (true))) {
        removeCliSocket (id);
        QJsonObject callJson;
        callJson ["method"] = method;
        callJson ["args"] = args;

        if (!object.isEmpty ())
            callJson ["object"] = object;
#ifdef WEBSOCKET
        //theCliSocket->write (QJsonDocument (callJson).toJson ());
        theCliSocket->sendBinaryMessage (QJsonDocument (callJson).toJson ());
#else
        theCliSocket->write (QJsonDocument (callJson).toBinaryData ());
#endif

        //theCliSocket->waitForBytesWritten (); //BUG QWebsocket缺少此函数的实现
     }

    return new cliRawDataTrans (theCliSocket);
}

void BGMRPCInterface::removeCliSocket(qulonglong id)
{
    qDebug () << "closed";
    cliDataTrans* theDataTrans = CliDataTrans.take (id);
    theDataTrans->deleteLater ();
}

bool BGMRPCInterface::connectToHost(__socket* socket)
{
    // TODO 尚未实现-客户端连接服务端的处理
#ifdef WEBSOCKET
    socket->setHost (Hosts);
#endif
    if (Proxy.type () != QNetworkProxy::NoProxy)
        socket->setProxy (Proxy);
    socket->connectToHost (ServerAddress, ServerPort);

    return socket->waitForConnected ();
}

}
