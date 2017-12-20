#ifndef RPCCTRLPLUGIN_H
#define RPCCTRLPLUGIN_H

#include "rpcctrlplugin_global.h"
#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <QObject>
#include <relatedclient.h>
#include <bgmrobjectstorage.h>

using namespace BGMircroRPCServer;
class RPCCtrlObj;
class RPCCtrlAdaptor;
//class BGMRPC;

class RPCCtrlObj : public BGMRObject < RPCCtrlAdaptor >
{
public:
    RPCCtrlObj ();

    QString objectType () const;
    QJsonArray objectMethods (BGMRClient*, const QJsonArray& args);
    QJsonArray RPCObjects (BGMRClient*, const QJsonArray&);
    QJsonArray RPCCreateObject (BGMRClient*, const QJsonArray& args);
    QJsonArray loadPlugin (BGMRClient*, const QJsonArray& args);
    QJsonArray RPCTypes (BGMRClient*, const QJsonArray&);
    QJsonArray setPassword (BGMRClient*, const QJsonArray& args);
    QJsonArray login (BGMRClient* p, const QJsonArray& args);
    QJsonArray loginout (BGMRClient* p, const QJsonArray&);
    QJsonArray useObject (BGMRClient* p, const QJsonArray& args);
    QJsonArray setRootDir (BGMRClient*, const QJsonArray& args);
    QJsonArray setPluginDir (BGMRClient*, const QJsonArray& args);

    void setRPC (BGMRPC* rpc);
    bool clientIdentify (BGMRClient* p,
                         const QString& method,
                         const QJsonArray&, bool /*lc*/);

protected:
    relatedClients RelClients;
    BGMRPC* RPC;
    QStringList publicMethods;
};

class RPCCtrlAdaptor : public BGMRAdaptor < RPCCtrlObj >
{
public:
    RPCCtrlAdaptor () { registerMethods (); }

    void registerMethods ();
};

extern "C" {
BGMRObjectInterface* objCreator ();
QString objType ();
bool initial (BGMRObjectStorage* storage, BGMRPC* rpc);
}

#endif // RPCCTRLPLUGIN_H
