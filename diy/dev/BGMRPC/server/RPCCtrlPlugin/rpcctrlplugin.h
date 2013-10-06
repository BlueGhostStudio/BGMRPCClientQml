#ifndef RPCCTRLPLUGIN_H
#define RPCCTRLPLUGIN_H

#include "rpcctrlplugin_global.h"
#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <QObject>
#include <relatedproc.h>
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
    QJsonArray objectMethods (BGMRProcedure*, const QJsonArray& args);
    QJsonArray RPCObjects (BGMRProcedure*, const QJsonArray&);
    QJsonArray RPCCreateObject (BGMRProcedure*, const QJsonArray& args);
    QJsonArray loadPlugin (BGMRProcedure*, const QJsonArray& args);
    QJsonArray RPCTypes (BGMRProcedure*, const QJsonArray&);
    QJsonArray setPassword (BGMRProcedure*, const QJsonArray& args);
    QJsonArray login (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray loginout (BGMRProcedure* p, const QJsonArray&);
    QJsonArray useObject (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray setRootDir (BGMRProcedure*, const QJsonArray& args);
    QJsonArray setPluginDir (BGMRProcedure*, const QJsonArray& args);

    void setRPC (BGMRPC* rpc);
    bool procIdentify (BGMRProcedure*p, const QJsonObject& call);

protected:
    relatedProcs RelProcs;
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
