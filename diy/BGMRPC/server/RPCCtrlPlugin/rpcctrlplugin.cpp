#include "rpcctrlplugin.h"
#include <bgmrpc.h>
#include "socket.h"
#include <QHostAddress>

RPCCtrlObj::RPCCtrlObj()
{
    publicMethods << "login";
}

QString RPCCtrlObj::objectType() const
{
    return objType ();
}

QJsonArray RPCCtrlObj::objectMethods(BGMRProcedure*, const QJsonArray& args)
{
    if (!RPC)
        return QJsonArray ();

    QJsonArray result;
    BGMRObjectInterface* obj = RPC->object (args [0].toString ());
    if (obj) {
        QStringList methodList = obj->adaptor ()->methods ();
        result.append (QJsonArray::fromStringList (methodList));
    }

    return result;
}

QJsonArray RPCCtrlObj::RPCObjects(BGMRProcedure*, const QJsonArray&)
{
    if (!RPC)
        return QJsonArray ();

    QJsonArray result;
    QList < BGMRObjectInterface* > objList
            = RPC->objectStorage ()->objects ();

    QJsonArray retObjsList;
    foreach (BGMRObjectInterface* obj, objList) {
        QJsonObject retObj;
        retObj ["type"] = obj->objectType ();
        retObj ["name"] = obj->objectName ();
        retObjsList.append (retObj);
    }

    result.append (retObjsList);
    return result;
}

QJsonArray RPCCtrlObj::RPCCreateObject(BGMRProcedure*, const QJsonArray& args)
{
    if (!RPC)
        return QJsonArray ();

    QString type = args[0].toString ();
    QString name = args[1].toString ();

    bool ok = RPC->objectStorage ()->installObject (name, type);

    QJsonArray result;
    result.append (ok);

    return result;
}

QJsonArray RPCCtrlObj::loadPlugin(BGMRProcedure*, const QJsonArray& args)
{
    if (!RPC)
        return QJsonArray ();

    BGMRObjectStorage* theObjectStorage = RPC->objectStorage ();
    bool ok = theObjectStorage->installPlugin (args [0].toString ());

    QJsonArray result;
    result.append (ok);
    return result;
}

QJsonArray RPCCtrlObj::RPCTypes(BGMRProcedure*, const QJsonArray&)
{
    if (!RPC)
        return QJsonArray ();

    QJsonArray result;
    result.append (QJsonArray::fromStringList (
                       RPC->objectStorage ()->types ()));

    return result;
}

QJsonArray RPCCtrlObj::setPassword(BGMRProcedure*, const QJsonArray& args)
{
    BGMRPC::Settings->setValue ("password", args [0]);

    return QJsonArray ();
}

QJsonArray RPCCtrlObj::login(BGMRProcedure* p, const QJsonArray& args)
{
    bool allow = false;
    if (BGMRPC::Settings->contains ("password")) {
        if (BGMRPC::Settings->value ("password").toString ()
                == args [0].toString ())
            allow = true;
    } else
        allow = true;

    p->privateData (this, "allow") = allow;

    QJsonArray ret;
    ret.append (allow);

    return ret;
}

QJsonArray RPCCtrlObj::loginout(BGMRProcedure* p, const QJsonArray&)
{
    p->privateData (this, "allow") = false;

    return QJsonArray ();
}

QJsonArray RPCCtrlObj::useObject(BGMRProcedure* p, const QJsonArray& args)
{
    if (!RPC)
        return QJsonArray ();
    QString objName = args [0].toString ();
    BGMRObjectInterface* object = RPC->object (objName);
    p->setObject (object);

    QJsonArray result;
    if (object)
        result.append (true);
    else
        result.append (false);

    return result;
}

QJsonArray RPCCtrlObj::setRootDir(BGMRProcedure*, const QJsonArray& args)
{
    QString pluginDir = args [0].toString ("~/.BGMR");
    pluginDir.remove (QRegExp ("/\\s*$"));
    pluginDir.replace (QRegExp ("^\\s*~"), QDir::homePath ());
    BGMRPC::Settings->setValue ("rootDir", pluginDir);

    return QJsonArray ();
}

QJsonArray RPCCtrlObj::setPluginDir(BGMRProcedure*, const QJsonArray& args)
{
    QString pluginDir = args [0].toString ("~/.BGMR/plugin");
    pluginDir.remove (QRegExp ("/\\s*$"));
    pluginDir.replace (QRegExp ("^\\s*~"), QDir::homePath ());
    BGMRPC::Settings->setValue ("pluginDir", pluginDir);

    return QJsonArray ();
}

void RPCCtrlObj::setRPC(BGMRPC* rpc)
{
    RPC = rpc;
}

bool RPCCtrlObj::procIdentify(BGMRProcedure*p, const QJsonObject& call)
{
    if (publicMethods.contains (call ["method"].toString ()))
        return true;
    else {
        const QJsonValue allow = p->privateData (this, "allow");
        return allow.toBool (false);
    }
}


void RPCCtrlAdaptor::registerMethods()
{
    Methods [  "objectMethods"] = &RPCCtrlObj::objectMethods; //
    Methods [     "RPCObjects"] = &RPCCtrlObj::RPCObjects; //
    Methods ["RPCCreateObject"] = &RPCCtrlObj::RPCCreateObject;
    Methods [       "RPCTypes"] = &RPCCtrlObj::RPCTypes; //
    Methods [     "loadPlugin"] = &RPCCtrlObj::loadPlugin; //
    Methods [    "setPassword"] = &RPCCtrlObj::setPassword;
    Methods [          "login"] = &RPCCtrlObj::login;
    Methods [         "logout"] = &RPCCtrlObj::loginout;
    Methods [            "use"] = &RPCCtrlObj::useObject;
    Methods [     "setRootDir"] = &RPCCtrlObj::setRootDir; //
    Methods [   "setPluginDir"] = &RPCCtrlObj::setPluginDir; //
}

BGMRObjectInterface* objCreator()
{
    return NULL;
    //return new RPCCtrlObj (rpc);
}


QString objType()
{
    return QString ("RPCCtrl");
}


bool initial(BGMRObjectStorage* storage, BGMRPC* rpc)
{
    RPCCtrlObj* theCtrlObj = new RPCCtrlObj;
    theCtrlObj->setRPC (rpc);
    storage->installObject ("ctrl", theCtrlObj);

    return true;
}
