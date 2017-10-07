#ifndef HTTPD_H
#define HTTPD_H

#include "bgresourceplugin_global.h"

#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <QObject>
#include <relatedclient.h>
#include <bgmrobjectstorage.h>
#include <qhttpserver.hpp>
#include <QtSql>

using namespace BGMircroRPCServer;
using namespace qhttp::server;
class bgresObj;
class bgresAdptor;

class bgresObj : public BGMRObject < bgresAdptor >
{
public:
    bgresObj ();

    QString objectType () const;
    void start();

    QJsonArray addRes (BGMRClient*, const QJsonArray& args);
    QJsonArray removeRes (BGMRClient*, const QJsonArray& args);
    QJsonArray renameRes (BGMRClient*, const QJsonArray& args);
    QJsonArray listRes (BGMRClient*cli, const QJsonArray&);

private:
    QHttpServer ResHttpServer;
    QSqlDatabase ResDatabase;
    relatedClients RelClients;
};

class bgresAdptor : public BGMRAdaptor < bgresObj > {
public:
    bgresAdptor () { registerMethods (); }

    void registerMethods ();
};

extern "C" {
BGMRObjectInterface* objCreator ();
QString objType ();
bool initial (BGMRObjectStorage* storage, BGMRPC*);
}

#endif // HTTPD_H
