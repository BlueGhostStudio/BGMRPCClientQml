#include "jsobjs.h"

#include <QDebug>
#include <QFile>

BGMRObjectInterface* toRpcObj (const QJSValue& obj) {
    return qobject_cast < JsRpcObj* > (obj.toQObject())->rpcObj();
}

JsClient::JsClient(BGMRClient* cli, QObject *parent)
    : QObject(parent), Cli (cli)
{

}

qulonglong JsClient::cliID() const
{
    return Cli->cliID();
}

void JsClient::setPrivateData(const QJSValue& obj, const QString& key,
                            const QJsonValue& value)
{
    Cli->setPrivateDataJson(toRpcObj(obj), key, value);
}

QJsonValue JsClient::privateData(const QJSValue& obj, const QString& key) const
{
    return Cli->privateDataJson(toRpcObj(obj), key);
}

/*QJsonValue JsClient::callMethod(const QJSValue& obj,
                              const QString& method,
                              const QJsonArray& args) const
{
    Cli->callMethod(toRpcObj(obj), method, args);

    return QJsonValue ();
}*/

void JsClient::emitSignal(const QJSValue& obj, const QString& signal,
                        const QJsonArray& args) const
{
    Cli->emitSignal(toRpcObj(obj), signal, args);
}

void JsClient::close()
{
    Cli->close();
}

BGMRClient*JsClient::client()
{
    return Cli;
}

JsRpcObj::JsRpcObj(BGMRObjectInterface* obj, QObject* parent)
    : QObject (parent), RpcObj (obj)
{

}

BGMRObjectInterface* JsRpcObj::rpcObj() const
{
    return RpcObj;
}

QString JsRpcObj::objectName() const
{
    return RpcObj->objectName();
}

QString JsRpcObj::objectType() const
{
    return RpcObj->objectType();
}

QStringList JsRpcObj::objectMethods() const
{
    return RpcObj->adaptor()->methods();
}

QJsonArray JsRpcObj::callMethod(const QJSValue& cli, const QString& method,
                                const QJsonArray& args)
{
    return RpcObj->callMethod (
                qobject_cast < JsClient* >(cli.toQObject ())->client (),
                method, args, true);
}

JsJSObj::JsJSObj(BGMRObjectInterface* obj, QObject* parent)
    : JsRpcObj (obj, parent)
{

}

QString JsJSObj::objPath () const
{
    return toJsEngine ()->objPath ();
}


void JsJSObj::setGlobalMutexLock(bool lock) const
{
    toJsEngine()->setGlobalMutexLock(lock);
}

bool JsJSObj::globalMutexLock() const
{
    return toJsEngine()->globalMutexLock();
}

void JsJSObj::mutexLock() const
{
    mutex.lock ();
    qDebug () << "vvv mutexLock vvv";
}

void JsJSObj::mutexUnlock() const
{
    qDebug () << "^^^ mutexUnlock ^^^";
    mutex.unlock ();
}

void JsJSObj::addRelClient(const QJSValue& cli) const
{
    JsClient* _cli = qobject_cast < JsClient* > (cli.toQObject ());
    toJsEngine ()->relClients ()->addClient (_cli->client ());
}

bool JsJSObj::removeClient(const QJSValue& cli) const
{
    JsClient* _cli = qobject_cast < JsClient* > (cli.toQObject ());
    return toJsEngine ()->relClients ()
            ->removeClient (_cli->client ()->cliID ());
}

QJSValue JsJSObj::relClients(bool autoDel) const
{
    QJSEngine* engine = qjsEngine (this);
    QJSValue ret = engine->newArray ();

    QMap < qulonglong, BGMRClient* > _clients
            = toJsEngine ()->relClients ()->clients ();
    QMap < qulonglong, BGMRClient* >::const_iterator it;
    int i = 0;
    for (it = _clients.constBegin ();
         it != _clients.constEnd (); ++it) {
        JsClient* retItem = new JsClient (it.value ());
        if (autoDel)
            retItem->deleteLater ();
        ret.setProperty (i, engine->newQObject (retItem));
        i++;
    }

    return ret;
}

QJSValue JsJSObj::relClient(int cliID, bool autoDel) const
{
    JsClient* _client
            = new JsClient (toJsEngine ()->relClients ()->client (cliID));
    if (autoDel)
        _client->deleteLater ();
    return qjsEngine (this)->newQObject (_client);
}

void JsJSObj::onRelClientRemoved(const QJSValue& handle)
{
    RelClientRemovedHandle = handle;
    relatedClients* rcs = toJsEngine ()->relClients ();
    QObject::connect (rcs, &relatedClients::removedClient, [=](BGMRClient* cli) {
        if (RelClientRemovedHandle.isCallable ()) {
            JsClient* _client = new JsClient (cli);
            _client->deleteLater ();
            RelClientRemovedHandle.call (QJSValueList ()
                         << qjsEngine (this)->newQObject (_client));
        }
    });
}

bool JsJSObj::containsRelClient(const QJSValue& cli) const
{
    JsClient* _cli = qobject_cast < JsClient* > (cli.toQObject ());
    return toJsEngine ()->relClients ()->clients ().contains (_cli->cliID ());
}

void JsJSObj::emitSignal(const QString& signal, const QJsonArray& args) const
{
    toJsEngine ()->relClients ()->emitSignal (rpcObj (), signal, args);
}

bool JsJSObj::include(const QString& scrFileName) const
{
    QString _objPath = objPath ();

    QString error;
    bool ok = false;

    QFile scrFile (_objPath + scrFileName);
    if (!scrFile.open (QIODevice::ReadOnly))
        error = QString ("%1 can't open").arg (scrFileName);
    else {
        QString scrContent = scrFile.readAll ();
        scrFile.close ();

        QJSValue result = qjsEngine (this)->evaluate (scrContent, scrFileName);
        if (result.isError ()) {
            error = QString ("%0,%1: %2").arg (scrFileName)
                    .arg (result.property ("lineNumber").toInt ())
                    .arg (result.toString ());
        } else
            ok = true;
    }

    if (!ok)
        qDebug () << "loadScript Error: " << error;

    return ok;
}

bool JsJSObj::loadScript(const QString& scrFileName,
                         const QJSValue& arg0,
                         const QJSValue& arg1,
                         const QJSValue& arg2,
                         const QJSValue& arg3,
                         const QJSValue& arg4,
                         const QJSValue& arg5,
                         const QJSValue& arg6,
                         const QJSValue& arg7,
                         const QJSValue& arg8,
                         const QJSValue& arg9) const
{
    auto toJsValue = [=](const QJSValue& arg) {
        if (arg.isNumber ())
            return QJSValue (arg.toNumber ());
        else if (arg.isBool ())
            return QJSValue (arg.toBool ());
        else if (arg.isString ())
            return QJSValue (arg.toString ());

        return QJSValue ();
    };

    QString error;
    QJSValueList args;
    args << toJsValue (arg0) << toJsValue (arg1) << toJsValue (arg2)
         << toJsValue (arg3) << toJsValue (arg4) << toJsValue (arg5)
         << toJsValue (arg6) << toJsValue (arg7) << toJsValue (arg8)
         << toJsValue (arg9);
    bool ok = toJsEngine ()->loadJSFile (objPath () + scrFileName,
                                         error, args);

    if (!ok)
        qDebug () << "Error: " << error;

    return ok;
}

void JsJSObj::loadModule(const QString& module) const
{
    toJsEngine ()->loadModule (module);
}

QJSValue JsJSObj::toJS(const QJSValue& obj, bool autoDel) const
{
    if (obj.property ("__TYPE__").toString () == "JSEngine") {
        BGMRObjectInterface* _obj
                = qobject_cast < JsRpcObj* > (obj.toQObject ())->rpcObj ();
        JsEngine* jsObj = dynamic_cast < JsEngine* > (_obj);
        JsJSObj* _jsObj = new JsJSObj (jsObj);
        if (autoDel)
            _jsObj->deleteLater ();
        return qjsEngine (this)->newQObject (_jsObj);
    } else
        return QJSValue ();
}

JsEngine* JsJSObj::toJsEngine() const
{
    return dynamic_cast < JsEngine* > (RpcObj);
}

JsRPC::JsRPC(BGMRPC* rpc, QObject* parent)
    : QObject (parent), RPC (rpc)
{

}

QJSValue JsRPC::object(const QString& objName, bool autoDel) const
{
    BGMRObjectInterface* _obj = RPC->object (objName);
    if (_obj) {
        JsRpcObj* obj = new JsRpcObj (RPC->object (objName));
        if (autoDel)
            obj->deleteLater ();

        return qjsEngine (this)->newQObject (obj);
    } else
        return QJSValue ();
}

QJSValue JsRPC::objects(bool autoDel) const
{
    QJSEngine* engine = qjsEngine (this);
    QJSValue ret = engine->newArray ();

    int i = 0;
    foreach (BGMRObjectInterface* obj, RPC->objectStorage ()->objects ()) {
        JsRpcObj* _obj = new JsRpcObj (obj);
        if (autoDel)
            _obj->deleteLater ();
        ret.setProperty (i, engine->newQObject (_obj));
        i++;
    }

    return ret;
}

QStringList JsRPC::types() const
{
    return RPC->objectStorage ()->types ();
}

QJSValue JsRPC::createObject(const QString& objType,
                             const QString& objName,
                             bool autoDel) const
{
    QJSEngine* engine = qjsEngine (this);
    BGMRObjectInterface* obj
            = RPC->objectStorage ()->installObject (objName, objType);
    if (objType == "JSEngine") {
        JsEngine* theJsEngine = dynamic_cast < JsEngine* > (obj);
        theJsEngine->setRpc (RPC);
        theJsEngine->setObjPath (
                    engine->evaluate ("JS.objPath ()").toString ());
    }
    JsRpcObj* _obj = new JsRpcObj (obj);
    if (autoDel)
        _obj->deleteLater ();
    return engine->newQObject (_obj);
}

bool JsRPC::removeObject(const QJSValue& obj) const
{
    return RPC->objectStorage ()->removeObject (
                qobject_cast < JsRpcObj* > (obj.toQObject ())->objectName ());
}

bool JsRPC::installPlugin(const QString& pluginFileName) const
{
    return RPC->objectStorage ()->installPlugin (pluginFileName);
}
