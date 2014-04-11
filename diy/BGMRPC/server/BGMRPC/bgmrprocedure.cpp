#include "bgmrprocedure.h"
#include "bgmrpc.h"
#include "bgmrobject.h"
#include "bgmradaptor.h"
#include <QJsonDocument>
#include <QHostAddress>
#include <socket.h>

namespace BGMircroRPCServer {

QMutex mutex;

callThread::callThread(const QString& mID, BGMircroRPCServer::BGMRProcedure* p,
                       BGMRObjectInterface* o, const QString& m,
                       const QJsonArray& as, QObject* parent)
    : QThread (parent), MID (mID), OwnProc (p), Object (o), Method (m), Args (as)
{
    connect (this, SIGNAL(finished()), this,
             SLOT(deleteLater()), Qt::DirectConnection);
    connect (OwnProc, SIGNAL(destroyed()), this,
             SLOT(terminate()), Qt::DirectConnection);
}

void callThread::run()
{
    QJsonArray returnedValues;
    returnedValues
            = Object->adaptor ()->callMetchod (Object, OwnProc, Method, Args);

    OwnProc->returnValues (returnedValues, false, MID);
}

// =====================

qulonglong lastPID = 0;

BGMRProcedure::BGMRProcedure(BGMRPC* r, __socket* socket,
                             QObject *parent)
    : QObject(parent), RPC (r),
      ProcSocket (socket), SocketBuffer (socket, this),
      DirectSock (false)//, KeepConnected (false)
{
    PID = lastPID;
    lastPID++;
    connect (ProcSocket, SIGNAL(disconnected()),
             this, SLOT(onClientSocketDisconnected()));
    connect (&SocketBuffer, SIGNAL(readyRead()),
             this, SLOT(callMethod()));
    connect (this, SIGNAL(emitSignal(const BGMRObjectInterface*,
                                     QString, QJsonArray)),
             this, SLOT(onEmitSignal(const BGMRObjectInterface*,
                                     QString,QJsonArray)));
    connect (this, SIGNAL(returnValues(QJsonArray, bool, QString)),
             this, SLOT(onReturnValues(QJsonArray,bool,QString)));
}

BGMRProcedure::~BGMRProcedure()
{
//    ProcSocket->deleteLater ();
}

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

qulonglong BGMRProcedure::pID() const
{
    return PID;
}

void BGMRProcedure::close ()
{
    qDebug () << QObject::tr ("End the procedure (#%1) call.").arg(PID);

    //if (ProcSocket && ProcSocket->state () == QAbstractSocket::ConnectedState) {
    if (!DirectSock && ProcSocket->state () == QAbstractSocket::ConnectedState) {
        qDebug () << tr ("Close socket when procedure (#%1) end.").arg (PID);
        ProcSocket->disconnect ();
        ProcSocket->disconnectFromHost ();
        if (ProcSocket->waitForDisconnected ())
            ProcSocket->deleteLater ();
    }

    emit procExited (PID);
    qDebug () << QObject::tr ("Free the Procedure (#%1) memory.").arg (PID);
    deleteLater ();
}

void BGMRProcedure::setObject(BGMRObjectInterface* object)
{
    Object = object;
}

//bool BGMRProcedure::isKeepConnected() const
//{
//    return KeepConnected;
//}

__socket* BGMRProcedure::procSocket() const
{
    return ProcSocket;
}

__socket* BGMRProcedure::switchDirectSocket()
{
    disconnect (&SocketBuffer, SIGNAL(readyRead()));
    DirectSock = true;

    return ProcSocket;
}

bool BGMRProcedure::isDirectSocket() const
{
    return DirectSock;
}

BGMRPCSocketBuffer* BGMRProcedure::socketBuffer()
{
    return &SocketBuffer;
}

void BGMRProcedure::switchProcedure()
{
    DirectSock = false;
    disconnect (&SocketBuffer, SIGNAL(readyRead()));
    connect (&SocketBuffer, SIGNAL(readyRead()),
             this, SLOT(callMethod()));
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

void BGMRProcedure::onReturnValues (const QJsonArray& values,
                                  bool directSocketReturn,
                                  const QString& mID)
{
    if (!DirectSock || directSocketReturn) {
        QJsonObject jsonValues;
        jsonValues ["type"] = QString ("return");
        jsonValues ["values"] = values;
        jsonValues ["pID"] = (double)PID;
        jsonValues ["switchDirect"] = DirectSock;
        if (!DirectSock && !mID.isEmpty ())
            jsonValues ["mID"] = mID;

        mutex.lock ();
#ifdef WEBSOCKET
        ProcSocket->write (QString::fromUtf8 (QJsonDocument (jsonValues).toJson ()));
#else
        ProcSocket->write (QJsonDocument (jsonValues).toBinaryData ());
#endif
        mutex.unlock ();
    }
}

void BGMRProcedure::onEmitSignal(const BGMRObjectInterface* obj,
                               const QString& signal,
                               const QJsonArray& args)
{
    //if (ProcSocket) {
    if (!DirectSock) {
        QJsonObject jsonValues;
        jsonValues ["type"] = QString ("signal");
        jsonValues ["object"] = obj->objectName ();
        jsonValues ["signal"] = signal;
        jsonValues ["args"] = args;

        mutex.lock ();
#ifdef WEBSOCKET
        ProcSocket->write (QString::fromUtf8 (QJsonDocument (jsonValues).toJson ()));
#else
        ProcSocket->write (QJsonDocument (jsonValues).toBinaryData ());
#endif
        mutex.unlock ();
    }
}

void BGMRProcedure::onClientSocketDisconnected()
{
    qDebug () << tr ("On client disconnected");
    ProcSocket->deleteLater ();
    emit procExited (PID);
    qDebug () << QObject::tr ("Free the Procedure (#%1) memory.").arg (PID);
    deleteLater ();
}

void BGMRProcedure::callMethod ()
{
    if (!DirectSock) {
        QByteArray callJson = SocketBuffer.readAll ();

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
        QString mID = callJsonObj ["mID"].toString ();

        if (!objName.isEmpty ())
            Object = RPC->object (objName);

        if (Object) {
            qDebug () << QObject::tr ("Client (%1:%2) call (%3) Object's method (%4).")
                         .arg (ProcSocket->peerAddress ().toString (),
                               QString::number(ProcSocket->peerPort ()), Object->objectName (), methodName);
            ProcSocket->setParent (this);
            callThread* newCallThread = new callThread (mID, this, Object,
                                                        methodName, args);
//            connect (newCallThread, SIGNAL(finished()), newCallThread,
//                     SLOT(deleteLater()), Qt::DirectConnection);
//            connect (this, SIGNAL(destroyed()), newCallThread,
//                     SLOT(terminate()), Qt::DirectConnection);
            newCallThread->start ();
        } else
            qDebug () << QObject::tr ("No any object be used.");

    }
}
}
