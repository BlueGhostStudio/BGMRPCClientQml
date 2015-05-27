#include "bgmrpc.h"
#include "proc.h"
#include "step.h"

BGMRPC::BGMRPC(QQuickItem *parent) :
    QQuickItem(parent), CliSocket (new __socket),
    SocketBuffer (CliSocket), CSID (0), DirectSocket (false)
{
    connect (CliSocket, SIGNAL(connected()), SIGNAL(connected()));
    connect (CliSocket, SIGNAL(connected()), SIGNAL(connectStateChanged()));
    connect (CliSocket, SIGNAL(disconnected()), SIGNAL(disconnected()));
    connect (CliSocket, SIGNAL(disconnected()), SIGNAL(connectStateChanged()));
    connect (CliSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             SLOT(onError(QAbstractSocket::SocketError)));
    connect (&SocketBuffer, SIGNAL(readyRead()), SLOT(onDataRevice()));
    connect (CliSocket, SIGNAL(bytesWritten(qint64)),
             SIGNAL(dataWriten(qint64)));
}

void BGMRPC::setHost(const QString& h)
{
    Host = h;
}

QString BGMRPC::host() const
{
    return Host;
}

void BGMRPC::setPort(int p)
{
    Port = p;
}

int BGMRPC::port() const
{
    return Port;
}

void BGMRPC::connectToHost(const QString& host, quint16 port)
{
    Host = host;
    Port = port;

    connectToHost ();
}

void BGMRPC::connectToHost()
{
#ifdef WEBSOCKET
    QUrl serUrl;
    serUrl.setHost (Host);
    serUrl.setPort (Port);
    CliSocket->open (serUrl);
    CliSocket->setHost (Host);
#else
    QHostAddress serverAddress;
    if (Host.contains (QRegExp ("\\d\\.\\d.\\d.\\d")))
        serverAddress.setAddress (Host);
    else
        serverAddress = QHostInfo::fromName (Host).addresses ().first ();

    CliSocket->connectToHost (serverAddress, Port);
#endif
//    if (Proxy.type () != QNetworkProxy::NoProxy)
//        CliSocket->setProxy (Proxy);
}

void BGMRPC::close()
{
    CliSocket->disconnectFromHost ();
}

BGMRPC* BGMRPC::newInstance() const
{
    BGMRPC* newRPC = new BGMRPC (parentItem ());
    newRPC->setHost (Host);
    newRPC->setPort (Port);

    return newRPC;
}

bool BGMRPC::isConnected() const
{
    return CliSocket->state () == QAbstractSocket::ConnectedState;
}

bool BGMRPC::isDirectSocket() const
{
    return DirectSocket;
}

QQmlListProperty<proc> BGMRPC::procs()
{
    return QQmlListProperty < proc > (this, 0, &BGMRPC::appendProc, 0, 0, 0);
}

proc* BGMRPC::newProc()
{
    return new proc (this);
}

void BGMRPC::call(_RStep* aStep, const QString& object,
                   const QString& method,
                   const QJSValue& args)
{
    if (CliSocket->state () == QAbstractSocket::ConnectedState) {
        QString mID = object + '_' + method + QString::number (CSID);
        CallingSteps [mID] = aStep;
        CSID++;

        QJsonObject callJsonObj;
        callJsonObj ["object"] = object;
        callJsonObj ["method"] = method;
        callJsonObj ["mID"] = mID;
        callJsonObj ["args"] = toJson (args);

#ifdef WEBSOCKET
        CliSocket->sendBinaryMessage (QJsonDocument (callJsonObj).toJson ());
        //CliSocket->write (QJsonDocument (callJsonObj).toJson ());
#else
        CliSocket->write (QJsonDocument (callJsonObj).toBinaryData ());
#endif
    }
}

void BGMRPC::sendRawData(const QByteArray& data)
{
    CliSocket->write (data);
}

void BGMRPC::onDataRevice()
{
    QByteArray data = SocketBuffer.readAll ();

    if (DirectSocket)
        emit dataReviced (data);
    else {
#ifdef WEBSOCKET
        QJsonObject retJsonObj = QJsonDocument::fromJson (data).object ();
#else
        QJsonObject retJsonObj = QJsonDocument::fromBinaryData (data).object ();
#endif

        if (retJsonObj["type"].toString () == "return") {
            if (retJsonObj ["switchDirect"].toBool ()) {
                DirectSocket = true;
                emit done (toJsValue (retJsonObj ["values"]));
            } else {
                QString mID = retJsonObj ["mID"].toString ();
                _RStep* theStep = CallingSteps.take (mID);
                QJSValue retData = toJsValue (retJsonObj ["values"]);
                theStep->doneCallBack (retData);
            }
        } else {
            QString obj = retJsonObj ["object"].toString ();
            QString sig = retJsonObj ["signal"].toString ();
            QJSValue args = toJsValue (retJsonObj ["args"]);

            emit message (obj, sig, args);
        }
    }
}

void BGMRPC::onError(QAbstractSocket::SocketError err)
{
    emit error (err, CliSocket->errorString ());
}

void BGMRPC::appendProc (QQmlListProperty < proc >* list, proc* p)
{
    BGMRPC* RPC = qobject_cast < BGMRPC* > (list->object);
    if (RPC)
        p->setParentItem (RPC);
}

QJsonValue BGMRPC::toJson(const QJSValue& jsv)
{
    if (jsv.isArray ()) {
        QJsonArray jsonArray;
        QJSValueIterator it (jsv);
        while (it.hasNext ()) {
            it.next ();
            if (it.name () != "length")
                jsonArray.append (toJson (it.value ()));
        }
        return jsonArray;
    } else if (jsv.isObject ()) {
        QJsonObject jsonObject;
        QJSValueIterator it (jsv);
        while (it.hasNext ()) {
            it.next ();
            jsonObject [it.name ()] = toJson (it.value ());
        }
        return jsonObject;
    } else {
        QJsonValue jsonValue;
        if (jsv.isBool ())
            jsonValue = jsv.toBool ();
        else if (jsv.isNumber ())
            jsonValue = jsv.toNumber ();
        else if (jsv.isNull ())
            jsonValue = QJsonValue (QJsonValue::Null);
        else if (jsv.isUndefined ())
            jsonValue = QJsonValue (QJsonValue::Undefined);
        else
            jsonValue = jsv.toString ();

        return jsonValue;
    }
}

QJSValue BGMRPC::toJsValue(const QJsonValue& json)
{
    QJSValue jsValue;
    switch (json.type ())
    {
    case QJsonValue::Array:
    {
        QJsonArray array = json.toArray ();
        jsValue = engine->newArray ();
        for (int i = 0; i < array.count (); i++)
            jsValue.setProperty (i, toJsValue (array[i]));

        break;
    }
    case QJsonValue::Object:
    {
        QJsonObject object = json.toObject ();
        jsValue = engine->newObject ();
        QJsonObject::const_iterator it;
        for (it = object.constBegin (); it != object.constEnd (); ++it)
            jsValue.setProperty (it.key (), toJsValue (it.value ()));

        break;
    }
    case QJsonValue::Bool:
        jsValue = json.toBool ();
        break;
    case QJsonValue::Double:
        jsValue = json.toDouble ();
        break;
    case QJsonValue::String:
        jsValue = json.toString ();
        break;
    case QJsonValue::Null:
        jsValue = QJSValue (QJSValue::NullValue);
        break;
    case QJsonValue::Undefined:
        jsValue = QJSValue (QJSValue::UndefinedValue);
        break;
    }

    return jsValue;
}
