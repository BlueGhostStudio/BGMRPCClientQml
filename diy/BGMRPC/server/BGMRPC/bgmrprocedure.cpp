#include "bgmrprocedure.h"
#include "bgmrpc.h"
#include "bgmrobject.h"
#include "bgmradaptor.h"
#include <QJsonDocument>
#include <QHostAddress>
#include <socket.h>

namespace BGMircroRPCServer {

qulonglong lastPID = 0;

BGMRProcedure::BGMRProcedure(BGMRPC* r, __socket* socket,
                             QObject *parent) :
    QObject(parent), RPC (r), ProcSocket (socket), KeepConnected (false)
{
    PID = lastPID;
    lastPID++;
    connect (ProcSocket, SIGNAL(disconnected()),
             this, SLOT(onClientSocketDisconnected()));
    connect (ProcSocket, SIGNAL(readyRead()),
             this, SLOT(callMethod()));
}

BGMRProcedure::~BGMRProcedure()
{
//    ProcSocket->deleteLater ();
}

//BGMRPC*BGMRProcedure::rpc() const
//{
//    return RPC;
//}

//QJsonValueRef BGMRProcedure::privateData(const QString& key)
//{
//    return PrivateData ["global"][key];
//}

//QJsonValue BGMRProcedure::privateData(const QString& key) const
//{
//    return PrivateData ["global"][key];
//}

QJsonValueRef BGMRProcedure::privateData(const BGMRObjectInterface* obj,
                                         const QString& key)
{
    return PrivateData [obj->objectName ()][key];
}

QJsonValue BGMRProcedure::privateData(const BGMRObjectInterface* obj,
                                      const QString& key) const
{
    return PrivateData [obj->objectName ()][key];
}

void BGMRProcedure::emitSignal(const BGMRObjectInterface* obj,
                               const QString& signal,
                               const QJsonArray& args) const
{
    if (ProcSocket) {
        QJsonObject jsonValues;
        jsonValues ["type"] = QString ("signal");
        jsonValues ["object"] = obj->objectName ();
        jsonValues ["signal"] = signal;
        jsonValues ["args"] = args;
#ifdef WEBSOCKET
        ProcSocket->write (QString::fromUtf8 (QJsonDocument (jsonValues).toJson ()));
#else
        ProcSocket->write (QJsonDocument (jsonValues).toBinaryData ());
#endif
    }
}

qulonglong BGMRProcedure::pID() const
{
    return PID;
}

void BGMRProcedure::close ()
{
    qDebug () << QObject::tr ("End the procedure (#%1) call.").arg(PID);

    if (ProcSocket && ProcSocket->state () == QAbstractSocket::ConnectedState) {
        qDebug () << tr ("Close socket when procedure (#%1) end.").arg (PID);
        ProcSocket->disconnect ();
        ProcSocket->disconnectFromHost ();
        if (ProcSocket->waitForDisconnected ())
            ProcSocket->deleteLater ();
    }

    emit procExited (PID);
    qDebug () << QObject::tr ("Free the Procedure (#%1) memory.").arg (PID);
    delete this;
}

void BGMRProcedure::setObject(BGMRObjectInterface* object)
{
    Object = object;
}

bool BGMRProcedure::isKeepConnected() const
{
    return KeepConnected;
}

__socket* BGMRProcedure::procSocket() const
{
    return ProcSocket;
}

__socket* BGMRProcedure::detachSocket()
{
    __socket* theSocket = ProcSocket;
    ProcSocket->disconnect ();
    ProcSocket = NULL;
    KeepConnected = false;

    return theSocket;
}

QJsonArray BGMRProcedure::callMethod(const QString& obj,
                                     const QString& method,
                                     const QJsonArray& args)
{
    BGMRObjectInterface* aObj = NULL;
    if (!obj.isEmpty ())
        aObj = RPC->object (obj);

    if (aObj)
        return aObj->adaptor ()->callMetchod (aObj, this, method, args);
    else
        return QJsonArray ();
}

void BGMRProcedure::onClientSocketDisconnected()
{
    qDebug () << tr ("On client disconnected");
    close ();
    if (ProcSocket)
        ProcSocket->deleteLater ();
//    emit procExited (PID);
//    qDebug () << QObject::tr ("Free the Procedure (#%1) memory.").arg (PID);
//    delete this;
}

void BGMRProcedure::callMethod ()
{
    if (ProcSocket) {
        QByteArray callJson = ProcSocket->readAll ();

#ifdef WEBSOCKET
        const QJsonObject callJsonObj
                = QJsonDocument::fromJson (callJson).object ();
#else
        QJsonObject callJsonObj
                = QJsonDocument::fromBinaryData (callJson).object ();
#endif

        QString objName  = callJsonObj ["object"].toString ();
        QString methodName = callJsonObj ["method"].toString ();
        QJsonArray args = callJsonObj ["args"].toArray ();
        KeepConnected = callJsonObj ["keepConnected"].toBool ();
        QString mID = callJsonObj ["mID"].toString ();

        if (!objName.isEmpty ())
            Object = RPC->object (objName);

        if (Object) {
            qDebug () << QObject::tr ("Client (%1:%2) call (%3) Object's method (%4).")
                         .arg (ProcSocket->peerAddress ().toString ())
                         .arg (ProcSocket->peerPort ())
                         .arg (Object->objectName ()).arg (methodName);
            QJsonArray returnedValues;
            if (!Object->procIdentify (this, callJsonObj)) {
                QJsonArray sigArgs;
                sigArgs.append (methodName);
                emitSignal (Object, "ERROR_ACCESS", sigArgs);
            } else
                returnedValues
                        = Object->adaptor ()->callMetchod (Object, this,
                                                           methodName, args);
            returnValues (returnedValues, mID);
        } else
            qDebug () << QObject::tr ("No any object be used.");
    }

    if (!KeepConnected || !ProcSocket)
        close ();
}

void BGMRProcedure::returnValues (const QJsonArray& values,
                                  const QString mID) const
{
    if (ProcSocket) {
        QJsonObject jsonValues;
        jsonValues ["type"] = QString ("return");
        jsonValues ["values"] = values;
        jsonValues ["pID"] = (double)PID;
        if (!mID.isEmpty ())
            jsonValues ["mID"] = mID;

#ifdef WEBSOCKET
        ProcSocket->write (QString::fromUtf8 (QJsonDocument (jsonValues).toJson ()));
#else
        ProcSocket->write (QJsonDocument (jsonValues).toBinaryData ());
#endif

        ProcSocket->waitForBytesWritten ();
    } else
        qDebug () << "no return";
}

}
