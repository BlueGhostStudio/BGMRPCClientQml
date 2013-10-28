#include "bgmrpcc.h"
#include <QDebug>
#include <QRegExp>

BGMRPCC::BGMRPCC(QJSEngine* e, QObject *parent) :
    QObject(parent), Engine (e), MID (0), CliSocket (new __socket)
{
    connect (CliSocket, SIGNAL(connected()), SLOT(onConnected()));
    connect (CliSocket, SIGNAL(disconnected()), SLOT(onClosed()));
    connect (CliSocket, SIGNAL(error(QAbstractSocket::SocketError)),
             SLOT(onError(QAbstractSocket::SocketError)));
    connect (CliSocket, SIGNAL(readyRead()),
             this, SLOT(dataRevice()));

    RPCThis = Engine->newQObject (this);
}

void BGMRPCC::testRet()
{
    QJSValue fun = Engine->evaluate("(function(a, b) { console.log ('hello world') })");
    fun.call();
}

QJSValue BGMRPCC::newMethod(const QString& obj, const QString& met, const QJSValue& args)
{
    QJSValue method = Engine->newObject ();
    method.setProperty ("RPC", RPCThis);

    method.setProperty ("object", obj);
    method.setProperty ("method", met);
    method.setProperty ("args", args);

    QJSValue doneFun = Engine->evaluate ("(function (dcb) {"
                                         "this.doneCallback = dcb;"
                                         "return this"
                                         "})");
    method.setProperty ("done", doneFun);

    QJSValue nextFun = Engine->evaluate ("(function (n) {"
                                         "this.nextCall = n;"
                                         "return n"
                                         "})");
    method.setProperty ("next", nextFun);

    QJSValue callFun = Engine->evaluate ("(function (m) {"
                                         "if (m == undefined) m = this;"
                                         "m.RPC.call (m)"
                                         "})");
    method.setProperty ("call", callFun);

    return method;
}

QJSValue BGMRPCC::newLMethod(const QJSValue& cb)
{
    QJSValue method = Engine->newObject ();
    method.setProperty ("RPC", RPCThis);

    method.setProperty ("callback", cb);

    QJSValue doneFun = Engine->evaluate ("(function (dcb) {"
                                         "this.doneCallback = dcb;"
                                         "return this"
                                         "})");
    method.setProperty ("done", doneFun);

    QJSValue failFun = Engine->evaluate ("(function (fcb) {"
                                         "this.failCallback = fcb;"
                                         "return this"
                                         "})");
    method.setProperty ("fail", failFun);

    QJSValue nextFun = Engine->evaluate ("(function (n) {"
                                         "this.nextCall = n;"
                                         "return n"
                                         "})");
    method.setProperty ("next", nextFun);

    QJSValue callFun = Engine->evaluate ("(function (m) {"
                                         "if (m == undefined) m = this;"
                                         "m.RPC.lcall (m)"
                                         "})");
    method.setProperty ("call", callFun);

    return method;
}

void BGMRPCC::call(const QJSValue& m)
{
    if (!m.isUndefined ()
            && CliSocket->state () == QAbstractSocket::ConnectedState) {
        QString objName = m.property ("object").toString ();
        QString methodName = m.property ("method").toString ();
        QString mID = objName
                      + methodName
                      + MID;
        MID++;
        CallingMethods [mID] = m;

        QJsonObject callJsonObj;
        callJsonObj ["object"] = objName;
        callJsonObj ["method"] = methodName;
        callJsonObj ["mID"] = mID;
        callJsonObj ["keepConnected"] = true;
        callJsonObj ["args"] = toJson (m.property ("args"));

        CliSocket->write (QJsonDocument (callJsonObj).toJson ());
    }
}

void BGMRPCC::lcall(const QJSValue& m)
{
    if (m.isUndefined ())
        return;

    QJSValue callback = m.property ("callback");
    if (callback.isCallable ()) {
        QJSValue ret = callback.call ();
        bool ok = true;
        bool isAbort = false;

        if (ret.isObject ()) {
            QJSValue ok_js = ret.property ("ok");
            QJSValue abort_js = ret.property ("abort");
            if (ok_js.isBool ())
                ok = ok_js.toBool ();
            if (abort_js.isBool ())
                isAbort = abort_js.toBool ();
        } else if (ret.isBool ())
            ok = ret.toBool ();

        if (ok) {
            QJSValue doneFun = m.property ("doneCallback");
            if (doneFun.isCallable ())
                doneFun.call ();
        } else {
            QJSValue failFun = m.property ("failCallback");
            if (failFun.isCallable ())
                failFun.call ();
        }

        if (!isAbort) {
            QJSValue nextCall = m.property ("nextCall");
            if (!nextCall.isUndefined ())
                nextCall.property ("call").call (QJSValueList () << nextCall);
        }
    }
}

void BGMRPCC::setHost(const QString& host)
{
    Host = host;
}

void BGMRPCC::setPort(quint16 port)
{
    Port = port;
}

void BGMRPCC::connectToHost(const QString& host, quint16 port)
{
    Host = host;
    Port = port;

    connectToHost ();
}

void BGMRPCC::connectToHost()
{
#ifdef WEBSOCKET
    CliSocket->setHost (Host);
#endif
    if (Proxy.type () != QNetworkProxy::NoProxy)
        CliSocket->setProxy (Proxy);

    QHostAddress serverAddress;
    if (Host.contains (QRegExp ("\\d\\.\\d.\\d.\\d")))
        serverAddress.setAddress (Host);
    else
        serverAddress = QHostInfo::fromName (Host).addresses ().first ();

    CliSocket->connectToHost (serverAddress, Port);
}

void BGMRPCC::close()
{
    CliSocket->disconnectFromHost ();
}

void BGMRPCC::setProxy(const QNetworkProxy& proxy)
{
    Proxy = proxy;
}

void BGMRPCC::setMessageCallback(const QJSValue& cb)
{
    MessageCallbacks ["-"] = cb;
}

void BGMRPCC::setMessageCallback(const QString& obj, const QJSValue& cb)
{
    MessageCallbacks [QString ("%1-").arg (obj)] = cb;
}

void BGMRPCC::setMessageCallback(const QString& obj, const QString& sig,
                                 const QJSValue cb)
{
    MessageCallbacks [QString ("%1-%2").arg (obj).arg (sig)] = cb;
}

void BGMRPCC::setOpenedFun(const QJSValue& jsv)
{
    OpenedFun = jsv;
}

QJSValue BGMRPCC::openedFun() const
{
    return OpenedFun;
}

void BGMRPCC::setClosedFun(const QJSValue& jsv)
{
    ClosedFun = jsv;
}

QJSValue BGMRPCC::closedFun() const
{
    return ClosedFun;
}

void BGMRPCC::setErrorFun(const QJSValue& jsv)
{
    ErrorFun = jsv;
}

QJSValue BGMRPCC::errorFun() const
{
    return ErrorFun;
}

bool BGMRPCC::isConnected()
{
    return CliSocket->state () == QAbstractSocket::ConnectedState;
}

void BGMRPCC::onConnected()
{
    emit connectStateChanged ();
    if (OpenedFun.isCallable ())
        OpenedFun.call ();
}

void BGMRPCC::onClosed()
{
    emit connectStateChanged ();
    if (ClosedFun.isCallable ())
        ClosedFun.call ();
}

void BGMRPCC::onError(QAbstractSocket::SocketError)
{
    if (ErrorFun.isCallable ())
        ErrorFun.call (QJSValueList () << CliSocket->errorString ());
}

void BGMRPCC::dataRevice()
{
    QByteArray data = CliSocket->readAll ();
    QJsonObject retJsonObj = QJsonDocument::fromJson (data).object ();

    if (retJsonObj["type"].toString () == "return") {
        QString mID = retJsonObj ["mID"].toString ();
        QJSValue jsMethod = CallingMethods.take (mID);

        QJSValue doneFun = jsMethod.property ("doneCallback");
        if (!doneFun.isUndefined () && doneFun.isCallable ()) {
            QJSValue retData = toJsValue (retJsonObj ["values"]);
            QJSValue ok = doneFun.call (QJSValueList () << retData);
            if (ok.isUndefined () || ok.toBool ()) {
                QJSValue nextCall = jsMethod.property ("nextCall");
                if (!nextCall.isUndefined ())
                    nextCall.property ("call").call (QJSValueList () << nextCall);
            }
        }
    } else {
        QString obj = retJsonObj ["object"].toString ();
        QString sig = retJsonObj ["signal"].toString ();
        QJSValue args = toJsValue (retJsonObj ["args"]);

        QJSValueList messArgs;
        QString obj_sig = obj + '-' + sig;
        QString obj_all = obj + "-";
        QString all ("-");
        QJSValue messCb;
        if (MessageCallbacks.contains (obj_sig)) {
            messArgs << args;
            messCb = MessageCallbacks [obj_sig];
        } else if (MessageCallbacks.contains (obj_all)) {
            messArgs << obj << args;
            messCb = MessageCallbacks [obj_all];
        } else if (MessageCallbacks.contains (all)) {
            messArgs << obj << sig << args;
            messCb = MessageCallbacks [all];
        }

        if (messCb.isCallable ())
            messCb.call (messArgs);
    }

}

QJsonValue BGMRPCC::toJson(const QJSValue& jsv)
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

QJSValue BGMRPCC::toJsValue(const QJsonValue& json)
{
    QJSValue jsValue;
    switch (json.type ())
    {
    case QJsonValue::Array:
    {
        QJsonArray array = json.toArray ();
        jsValue = Engine->newArray ();
        for (int i = 0; i < array.count (); i++)
            jsValue.setProperty (i, toJsValue (array[i]));

        break;
    }
    case QJsonValue::Object:
    {
        QJsonObject object = json.toObject ();
        jsValue = Engine->newObject ();
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
