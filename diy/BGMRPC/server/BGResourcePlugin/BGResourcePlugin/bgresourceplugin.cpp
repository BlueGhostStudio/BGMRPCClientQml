#include "bgresourceplugin.h"
#include <qhttpserverresponse.hpp>
#include <qhttpserverrequest.hpp>
#include <bgmrpc.h>
#include <QUrl>

bgresObj::bgresObj()
    : ResHttpServer (qApp)
{
    ResDatabase = QSqlDatabase::addDatabase ("QSQLITE", "BGRESOURCEDB");
    //ResDatabase.setDatabaseName ();
    ResDatabase.setDatabaseName (BGMRPC::Settings->value ("rootDir").toString ()
                 + "/ObjsData/BGRES.db");
    qDebug () << "open:" << ResDatabase.open ();
}

QString bgresObj::objectType() const
{
    return objType ();
}

void bgresObj::start()
{
    qDebug () << "Res http server started";
    ResHttpServer.listen( // listening on 0.0.0.0:8080
                   QHostAddress::Any, 8080,
                   [=](QHttpRequest* req, QHttpResponse* res) {
        QString resName = req->url ().toString ().remove (QRegExp ("^/"));
        qDebug () << resName;

        // the response body data
        QSqlQuery query (ResDatabase);
        query.prepare ("SELECT data FROM res WHERE name=:N");
        query.bindValue (":N", resName);
        query.exec ();
        //qDebug () << query.size ();
        //if (query.size () > 0) {
            query.first ();
            res->setStatusCode(qhttp::ESTATUS_OK);
            res->end (QByteArray::fromBase64 (query.value ("data").toByteArray ()));
        /*} else {
            res->setStatusCode(qhttp::ESTATUS_NOT_FOUND);
            res->end ();
        }*/
        // automatic memory management for req/res
    });
}

QJsonArray bgresObj::addRes(BGMRClient*, const QJsonArray& args)
{
    QString name = args[0].toString ();
    QString data = args[1].toString ();
    qDebug () << "in bgresObj::addRes" << name << data;

    QSqlQuery checkQuery (ResDatabase);
    checkQuery.prepare ("SELECT count(id) AS exist FROM res WHERE name=:N");
    checkQuery.bindValue (":N", name);
    checkQuery.exec ();
    checkQuery.first ();
    if (checkQuery.value ("exist").toInt () == 0) {
        qDebug () << "exist";
        QSqlQuery instQuery (ResDatabase);
        instQuery.prepare ("INSERT INTO res (name,data) VALUES (:N,:D)");
        instQuery.bindValue (":N", name);
        instQuery.bindValue (":D", data);
        qDebug () << instQuery.exec ();

        return QJsonArray ({ true,
                             QString ("http://127.0.0.1:8080/%1")
                                .arg (name) });
    } else {
        qDebug () << "exist";
        return QJsonArray (
                    {
                        false,
                        QString ("The %1 resource already exists").arg (name)
                    });
    }
}

// ==========

void bgresAdptor::registerMethods()
{
    Methods["addRes"] = &bgresObj::addRes;
}

// ==========

BGMRObjectInterface* objCreator ()
{
    return NULL;
}

QString objType () {
    return QString ("bgres");
}

bool initial (BGMRObjectStorage *storage, BGMRPC*)
{
    qDebug () << "initial httpd";
    bgresObj* theBgresObj = new bgresObj;
    storage->installObject ("bgres", theBgresObj);
    theBgresObj->start ();

    return true;
}
