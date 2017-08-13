#ifndef RPCFILEPLUGIN_H
#define RPCFILEPLUGIN_H

#include "rpcfileplugin_global.h"

#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <QObject>
#include <relatedproc.h>
#include <bgmrobjectstorage.h>
#include "filestream.h"
using namespace BGMircroRPCServer;
class RPCFileObj;
class RPCFileAdaptor;
//class BGMRPC;

class RPCFileObj : public BGMRObject < RPCFileAdaptor >
{
public:
    QString objectType () const;
    QJsonArray connect (BGMRClient* p, const QJsonArray&);
    QJsonArray download (BGMRClient* p, const QJsonArray& args);
    QJsonArray upload (BGMRClient* p, const QJsonArray& args);
    QJsonArray cp (BGMRClient*, const QJsonArray& args);
    QJsonArray rm (BGMRClient*, const QJsonArray& args);
    QJsonArray rename (BGMRClient*, const QJsonArray& args);
    QJsonArray dir (BGMRClient*proc, const QJsonArray& args);
    QJsonArray mkdir (BGMRClient*, const QJsonArray& args);
    QJsonArray rmdir (BGMRClient*, const QJsonArray& args);
    QJsonArray setPath (BGMRClient*, const QJsonArray& args);
    QJsonArray paths (BGMRClient*, const QJsonArray&);
    QJsonArray removePath (BGMRClient*, const QJsonArray& args);
    QJsonArray rootPath (BGMRClient*, const QJsonArray&);

    BGMRClient* proc (quint16 id) const;

protected:
    relatedClients RelProcs;

private:
	QString rootPath ();
    QString getPath (const QString& pathID);
};

class RPCFileAdaptor : public BGMRAdaptor < RPCFileObj >
{
public:
    RPCFileAdaptor () { registerMethods (); }

    void registerMethods ();
};

extern "C" {
BGMRObjectInterface* objCreator ();
QString objType ();
bool initial (BGMRObjectStorage* storage, BGMRPC* rpc);
}

#endif // RPCFILEPLUGIN_H
