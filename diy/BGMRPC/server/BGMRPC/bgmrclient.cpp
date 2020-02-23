#include "bgmrclient.h"
#include "bgmrpc.h"
#include "bgmrobject.h"
#include "bgmradaptor.h"
#include <QJsonDocument>
#include <QHostAddress>
#include <socket.h>
#include <parsejson.h>
#include <QPointer>

namespace BGMircroRPCServer
{

void defaultPDDeleter(void* data)
{
    delete data;
}

QMutex mutex;

callThread::callThread(const QString& mID, BGMircroRPCServer::BGMRClient* cli,
                       BGMRObjectInterface* o, const QString& m,
                       const QJsonArray& as, QObject* parent)
    : QThread(parent), MID(mID), OwnCli(cli), Object(o), Method(m), Args(as)
{
    connect(this, SIGNAL(finished()), this,
            SLOT(deleteLater()), Qt::DirectConnection);
}

void callThread::run()
{
    QJsonArray returnedValues;
    returnedValues
        = Object->adaptor()->callMetchod(Object, OwnCli, Method, Args, false);

    OwnCli->returnValues(returnedValues, false, MID);
}

// =====================

qulonglong lastCliID = 0;

BGMRClient::BGMRClient(BGMRPC* r, __socket* socket,
                       QObject *parent)
    : QObject(parent), RPC(r),
      ClientSocket(socket), SocketBuffer(socket, this),
      DirectSock(false), RefThreadCount(0), Exited(false)   //, KeepConnected (false)
{
    CliID = lastCliID;
    lastCliID++;
    connect(ClientSocket, SIGNAL(disconnected()),
            this, SLOT(onClientSocketDisconnected()));
    connect(&SocketBuffer, SIGNAL(readyRead()),
            this, SLOT(handleCallRequest()));
    connect(this, SIGNAL(emitSignal(const BGMRObjectInterface*,
                                    QString, QJsonArray)),
            this, SLOT(onEmitSignal(const BGMRObjectInterface*,
                                    QString, QJsonArray)));
    connect(this, SIGNAL(returnValues(QJsonArray, bool, QString)),
            this, SLOT(onReturnValues(QJsonArray, bool, QString)));

    ClientSocket->ping();
    connect(ClientSocket, &QWebSocket::pong, [ = ]() {
        QPointer < QWebSocket > socketPointer(ClientSocket);
        QTimer::singleShot(5000, [ = ]() {
            if (socketPointer)
                socketPointer->ping();
        });
    });
}

BGMRClient::~BGMRClient()
{
    qDebug() << QObject::tr("Free the Client (#%1) memory.").arg(CliID);
    ClientSocket->deleteLater();
}

void BGMRClient::setPrivateData(const BGMRObjectInterface* obj, const QString& name, void* data, void (*del)(void*))
{
    QString objName = obj->objectName();
    ObjPriDataType& objPriData = PrivateData[objName];
    objPriData[name] = PriDataType(data, del);
}

void* BGMRClient::privateData(const BGMRObjectInterface* obj, const QString& name)
{
    QString objName = obj->objectName();
    void* pd = NULL;
    if (PrivateData.contains(objName)) {
        ObjPriDataType objPriData = PrivateData[objName];
        if (objPriData.contains(name))
            pd = objPriData[name].first;
    }

    return pd;
}

QJsonValue BGMRClient::privateDataJson(const BGMRObjectInterface* obj, const QString& name)
{
    void* PD_ptr = privateData(obj, name);
    if (!PD_ptr)
        return QJsonValue();
    else
        return *(QJsonValue*)PD_ptr;
}

void BGMRClient::setPrivateDataJson(const BGMRObjectInterface* obj, const QString& name, const QJsonValue& value)
{
    void* PD_ptr = privateData(obj, name);
    if (!PD_ptr) {
        QJsonValue* priData = new QJsonValue(value);
        setPrivateData(obj, name, priData, PDDeleter < QJsonValue >);
    } else
        *(QJsonValue*)PD_ptr = value;
}

qulonglong BGMRClient::cliID() const
{
    return CliID;
}

bool BGMRClient::exited() const
{
    return Exited;
}

void BGMRClient::close()
{
    qDebug() << QObject::tr("End the Client (#%1) call.").arg(CliID);

    if (!DirectSock && ClientSocket->state() == QAbstractSocket::ConnectedState) {
        qDebug() << tr("Close socket when Client (#%1) end.").arg(CliID);
#ifdef WEBSOCKET
        ClientSocket->close();
#else
        ClientSocket->disconnect();
        ClientSocket->disconnectFromHost();
        if (ClientSocket->waitForDisconnected())
            ClientSocket->deleteLater();
#endif
    }

    emit clientExited(CliID);
    qDebug() << QObject::tr("Free the Client (#%1) memory.").arg(CliID);
    foreach (ObjPriDataType objPriData, PrivateData) {
        foreach (PriDataType priData, objPriData) {
            priData.second(priData.first);
        }
    }

    deleteLater();
}

void BGMRClient::setObject(BGMRObjectInterface* object)
{
    Object = object;
}

__socket* BGMRClient::clientSocket() const
{
    return ClientSocket;
}

__socket* BGMRClient::switchDirectSocket()
{
    disconnect(&SocketBuffer, SIGNAL(readyRead()));
    DirectSock = true;

    return ClientSocket;
}

bool BGMRClient::isDirectSocket() const
{
    return DirectSock;
}

BGMRPCSocketBuffer* BGMRClient::socketBuffer()
{
    return &SocketBuffer;
}

void BGMRClient::switchClient()
{
    DirectSock = false;
    disconnect(&SocketBuffer, SIGNAL(readyRead()));
    connect(&SocketBuffer, SIGNAL(readyRead()),
            this, SLOT(handleCallRequest()));
}

/*QJsonArray BGMRClient::callMethod(BGMRObjectInterface* obj,
                                  const QString& method,
                                  const QJsonArray& args)
{
    RefThreadCount++;
    qDebug () << "Internal calls, RefCount: " << RefThreadCount;
    QJsonArray ret = obj->adaptor ()->callMetchod (obj, this, method, args);
    RefThreadCount--;
    qDebug () << "Internal return, RefCount: " << RefThreadCount;
    return ret;
}*/

void BGMRClient::onReturnValues(const QJsonArray& values,
                                bool directSocketReturn,
                                const QString& mID)
{
    RefThreadCount--;
    qDebug() << "return, RefCount: " << RefThreadCount;
    if (RefThreadCount <= 0 && Exited)
        deleteLater();
    else if (!DirectSock || directSocketReturn) {
        QJsonObject jsonValues;
        jsonValues ["type"] = QString("return");
        jsonValues ["values"] = values;
        jsonValues ["cliID"] = (double)CliID;
        jsonValues ["switchDirect"] = DirectSock;
        if (!DirectSock && !mID.isEmpty())
            jsonValues ["mID"] = mID;

        mutex.lock();
#ifdef WEBSOCKET
        ClientSocket->sendTextMessage(QString::fromUtf8(QJsonDocument(jsonValues).toJson()));
#else
        ClientSocket->write(QJsonDocument(jsonValues).toBinaryData());
#endif
        mutex.unlock();
    }
}

void BGMRClient::onEmitSignal(const BGMRObjectInterface* obj,
                              const QString& signal,
                              const QJsonArray& args)
{
    if (!DirectSock) {
        QJsonObject jsonValues;
        jsonValues ["type"] = QString("signal");
        jsonValues ["object"] = obj->objectName();
        jsonValues ["signal"] = signal;
        jsonValues ["args"] = args;

        mutex.lock();
#ifdef WEBSOCKET
        ClientSocket->sendTextMessage(QString::fromUtf8(QJsonDocument(jsonValues).toJson()));
#else
        ClientSocket->write(QJsonDocument(jsonValues).toBinaryData());
#endif
        mutex.unlock();
    }
}

void BGMRClient::onClientSocketDisconnected()
{
    Exited = true;
    qDebug() << tr("On client disconnected");
    emit clientExited(CliID);

    qDebug() << "client disconnected, RefCount: " << RefThreadCount;
    if (RefThreadCount <= 0)
        deleteLater();
}

void BGMRClient::handleCallRequest()
{
    if (!DirectSock) {
        QByteArray callJson = SocketBuffer.readAll();

#ifdef WEBSOCKET
        const QJsonObject callJsonObj
            = QJsonDocument::fromJson(callJson).object();
        //const QJsonObject callJsonObj
        //        = parse (callJson).toObject ();
        //qDebug () << callJsonObj;
#else
        QJsonObject callJsonObj
            = QJsonDocument::fromBinaryData(callJson).object();
#endif

        QString objName  = callJsonObj ["object"].toString();
        QString methodName = callJsonObj ["method"].toString();
        QJsonArray args = callJsonObj ["args"].toArray();
        QString mID = callJsonObj ["mID"].toString();

        if (!objName.isEmpty())
            Object = RPC->object(objName);

        if (Object) {
            qDebug() << QObject::tr("Client (%1:%2) call (%3) Object's method (%4).")
                     .arg(ClientSocket->peerAddress().toString(),
                          QString::number(ClientSocket->peerPort()), Object->objectName(), methodName);
            ClientSocket->setParent(this);
            RefThreadCount++;
            qDebug() << "call by client, RefCount: " << RefThreadCount;
            callThread* newCallThread = new callThread(mID, this, Object,
                                                       methodName, args);
            //            connect (newCallThread, SIGNAL(finished()), newCallThread,
            //                     SLOT(deleteLater()), Qt::DirectConnection);
            //            connect (this, SIGNAL(destroyed()), newCallThread,
            //                     SLOT(terminate()), Qt::DirectConnection);
            newCallThread->start();
        } else
            qDebug() << QObject::tr("No any object be used.");

    }
}
}
