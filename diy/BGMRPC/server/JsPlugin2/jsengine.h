#ifndef JSENGINE_H
#define JSENGINE_H

#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <relatedclient.h>
#include <QThread>
#include <QJSEngine>
#include <bgmrobjectstorage.h>

#include "jsplugin2_global.h"

using namespace BGMircroRPCServer;

class JsEngine;
class JsEngineAdaptor;

class JsEngine : public BGMRObject < JsEngineAdaptor >
{
public:
    JsEngine ();
    ~JsEngine ();

    bool clientIdentify (BGMRClient* cli,
                       const QString& method, const QJsonArray& as);

    QString objectType() const;

    void setRpc (BGMRPC* rpc);
    BGMRPC* rpc () const;

    void setObjPath (const QString& path);
    QString objPath () const;
    bool loadJSFile (const QString& jsFileName, QString& error,
                     const QJSValueList& cstrArgs = QJSValueList ());
    void loadModule (const QString& module);

    QJsonArray js (BGMRClient* p, const QJsonArray& args);
    void setGlobalMutexLock (bool lock);
    bool globalMutexLock () const;

    relatedClients* relClients ();

private:
    QJSEngine* Engine;
    BGMRPC* RPC;
    bool GlobalMutexLock;
    relatedClients RelClients;
    QString ObjPath;

    QStringList LoadedModules;
};

class JsEngineAdaptor : public BGMRAdaptor < JsEngine >
{
public:
    JsEngineAdaptor () { registerMethods (); }
    void registerMethods ();
};

extern "C" {
BGMRObjectInterface* objCreator ();
QString objType ();
bool initial (BGMRObjectStorage* storage, BGMRPC*rpc);
}

#endif // JSPLUGIN2_H
