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
    QJsonArray connect (BGMRProcedure* p, const QJsonArray&);
    QJsonArray download (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray upload (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray cp (BGMRProcedure*, const QJsonArray& args);
    QJsonArray rm (BGMRProcedure*, const QJsonArray& args);
    QJsonArray rename (BGMRProcedure*, const QJsonArray& args);
    QJsonArray dir (BGMRProcedure*proc, const QJsonArray& args);
    QJsonArray mkdir (BGMRProcedure*, const QJsonArray& args);
    QJsonArray rmdir (BGMRProcedure*, const QJsonArray& args);
    QJsonArray setPath (BGMRProcedure*, const QJsonArray& args);
    QJsonArray paths (BGMRProcedure*, const QJsonArray&);
    QJsonArray removePath (BGMRProcedure*, const QJsonArray& args);
    QJsonArray rootPath (BGMRProcedure*, const QJsonArray&);

    BGMRProcedure* proc (quint16 id) const;

protected:
    relatedProcs RelProcs;

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
