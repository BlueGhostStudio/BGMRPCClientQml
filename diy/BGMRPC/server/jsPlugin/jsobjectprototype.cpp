#include "jsobjectprototype.h"
#include "jsplugin.h"
#include <bgmrpc.h>
#include <bgmrobjectstorage.h>

using namespace BGMircroRPCServer;

jsProcProto::jsProcProto(QObject* parent)
    : QObject (parent)
{

}

qulonglong jsProcProto::pID()
{
    return thisProc ()->pID ();
}

void jsProcProto::emitSignal(const BGMRObjectInterface* obj,
                             const QString& signal,
                             const QJsonArray& args)
{
    thisProc ()->emitSignal (obj, signal, args);
}

//void jsProcProto::setPrivateData(const QString& key, const QJsonValue& value)
//{
//    thisProc ()->privateData (key) = value;
//}

//QJsonValue jsProcProto::privateData(const QString& key) const
//{
//    return thisProc ()->privateData (key);
//}

void jsProcProto::setPrivateData(const BGMRObjectInterface* obj,
                                 const QString& key, const QJsonValue& value)
{
    thisProc ()->privateData (obj, key) = value;
}

QJsonValue jsProcProto::privateData(const BGMRObjectInterface* obj,
                                    const QString& key) const
{
    return thisProc ()->privateData (obj, key);
}

bool jsProcProto::isKeepConnected() const
{
    return thisProc ()->isKeepConnected ();
}

QJsonArray jsProcProto::callMethod(const QString& obj,
                                   const QString& method,
                                   const QJsonArray& args)
{
    return thisProc ()->callMethod (obj, method, args);
}

void jsProcProto::close()
{
    thisProc ()->close ();
}

procPtr jsProcProto::thisProc() const
{
    return qvariant_cast < BGMRProcedure* > (thisObject().data().toVariant ());
}


// =======================================================================


jsRPCObjectProto::jsRPCObjectProto(QObject* parent)
    : QObject (parent)
{

}

QString jsRPCObjectProto::objectName() const
{
    return thisRPCObj ()->objectName ();
}

QString jsRPCObjectProto::objectType() const
{
    return thisRPCObj ()->objectType ();
}

QStringList jsRPCObjectProto::objectMethods() const
{
    return thisRPCObj ()->adaptor ()->methods ();
}

QJsonArray jsRPCObjectProto::callMethod(BGMRProcedure* proc,
                                        const QString& method,
                                        const QJsonArray& args)
{
    BGMRObjectInterface* obj = thisRPCObj ();
    return obj->adaptor ()->callMetchod (obj, proc, method, args);
}

BGMRObjectInterface*jsRPCObjectProto::thisRPCObj() const
{
    return qvariant_cast < BGMRObjectInterface* > (thisObject().data().toVariant ());
}



// =======================================================================


jsJsObjProto::jsJsObjProto(QObject* parent)
    :jsRPCObjectProto (parent)
{
}

void jsJsObjProto::addProc(BGMRProcedure* proc)
{
    thisJsObj ()->relProcs ()->addProc (proc);
}

bool jsJsObjProto::removeProc(qulonglong pID)
{
    return thisJsObj ()->relProcs ()->removeProc (pID);
}

relProcsMap jsJsObjProto::relProcs() const
{
    return thisJsObj ()->relProcs ()->procs ();
}

BGMRProcedure* jsJsObjProto::relProc(qulonglong pID) const
{
    return thisJsObj ()->relProcs ()->proc (pID);
}

void jsJsObjProto::emitSignal(const QString& signal, const QJsonArray& args) const
{
    thisJsObj ()->relProcs ()->emitSignal (thisRPCObj (), signal, args);
}

BGMRObjectInterface* jsJsObjProto::thisRPCObj() const
{
    return qvariant_cast < jsObjPtr > (thisObject().data().toVariant ());
}

jsObjPtr jsJsObjProto::thisJsObj() const
{
    return static_cast < jsObjPtr > (thisRPCObj ());
}

// =====================================================================



jsRPC::jsRPC(QObject* parent)
    : QObject (parent)
{

}

RPCObjList jsRPC::objects() const
{
    RPCObjList theObjList;
    foreach (BGMRObjectInterface* obj, RPC->objectStorage ()->objects ())
        theObjList [obj->objectName ()] = obj;

    return theObjList;
}

QStringList jsRPC::types() const
{
    return RPC->objectStorage ()->types ();
}

bool jsRPC::creatorObject(const QString& type, const QString& objName)
{
    return RPC->objectStorage ()->installObject (objName, type);
}

bool jsRPC::installPlugin(const QString& pluginFileName)
{
    return RPC->objectStorage ()->installPlugin (pluginFileName);
}

void jsRPC::setRPC(BGMRPC* rpc)
{
    RPC = rpc;
}

// ======================================================================

jsSqlQueryProto::jsSqlQueryProto(QObject* parent)
    : QObject (parent)
{
}

void jsSqlQueryProto::close()
{
    QSqlDatabase::database ().close ();
}

bool jsSqlQueryProto::exec(const QString& q)
{
    return thisQuery ()->exec (q);
}

QScriptValue jsSqlQueryProto::fetch(const QString& q)
{
    QScriptEngine* theEngine = engine ();
    QSqlQuery* query = thisQuery ();
    bool ok = query->exec (q);

    QScriptValue jsResult = theEngine->newArray ();
    if (ok) {
        int index = 0;
        while (query->next ()) {
            jsResult.setProperty (index, record (query));
            index++;
        }
    }

    return jsResult;
}

bool jsSqlQueryProto::insertRecord(const QString& table, const QScriptValue& jsRec)
{
    QString valueStatem;
    QString colStatem;

    QScriptValueIterator it (jsRec);
    bool begin = true;
    while (it.hasNext ()) {
        it.next ();

        if (!begin) {
            valueStatem += ",";
            colStatem += ",";
        } else
            begin = false;

        colStatem += it.name ();
        valueStatem += toDBValue (it.value ());
    }

    QString qstr = QString ("INSERT INTO %1 (%2) VALUES (%3)").arg (table)
                   .arg (colStatem).arg (valueStatem);
    return thisQuery ()->exec (qstr);
}

bool jsSqlQueryProto::updateRecord(const QString& table, const QScriptValue& oldRec, const QScriptValue& newRec)
{
    QString qstr = "UPDATE " + table + " SET ";

    QScriptValueIterator it (newRec);
    bool begin = true;
    while (it.hasNext ()) {
        it.next ();
        if (!begin)
            qstr += ", ";
        else
            begin = false;

        qstr += it.name () + "=";
        qstr += toDBValue (it.value ());
    }

    qstr += whereStatem (oldRec);

    return thisQuery ()->exec (qstr);
}

//bool jsSqlQueryProto::deleteRecord(const QString& table, const QString& conditions)
//{
//    return thisQuery ()->exec (
//                QString ("DELETE FROM %1 WHERE %2")
//                .arg (table).arg (conditions));
//}

bool jsSqlQueryProto::deleteRecord(const QString& table, const QScriptValue& rec)
{
    QString qstr = "DELETE FROM " + table + whereStatem (rec);

    return thisQuery ()->exec (qstr);
}

bool jsSqlQueryProto::next()
{
    return thisQuery ()->next ();
}

bool jsSqlQueryProto::previous()
{
    return thisQuery ()->previous ();
}

bool jsSqlQueryProto::first()
{
    return thisQuery ()->first ();
}

bool jsSqlQueryProto::last()
{
    return thisQuery ()->last ();
}

bool jsSqlQueryProto::seek(int index)
{
    return thisQuery ()->seek (index);
}

QScriptValue jsSqlQueryProto::record(QSqlQuery* query) const
{
    QScriptEngine* theEngine = engine ();
    QScriptValue jsRec = theEngine->newObject ();

    if (!query)
        query = thisQuery ();
    QSqlRecord rec = query->record ();
    for (int i = 0; i < rec.count (); ++i) {
        jsRec.setProperty (rec.fieldName (i),
                           theEngine->toScriptValue (rec.value (i)));
    }

    return jsRec;
}

QString jsSqlQueryProto::lastError() const
{
    return thisQuery ()->lastError ().text ();
}

QSqlQuery*jsSqlQueryProto::thisQuery() const
{
    return qscriptvalue_cast< QSqlQuery* >(thisObject().data());
}

QString jsSqlQueryProto::toDBValue (const QScriptValue& fv)
{
    QString DBValue = fv.toString ();
    if (fv.isDate () || fv.isString ())
        DBValue =  "'" + escape (DBValue) + "'";
    else if (fv.isNull ())
        DBValue = "NULL";

    return DBValue;
}

QString jsSqlQueryProto::whereStatem(const QScriptValue& rec)
{
    QString whereStatem (" WHERE ");

    bool begin = true;
    QScriptValueIterator it (rec);
    while (it.hasNext ()) {
        it.next ();
        if (!begin)
            whereStatem += " AND ";
        else
            begin = false;

        whereStatem += it.name () + "=";

        whereStatem += toDBValue (it.value ());
    }

    return whereStatem;
}

QString jsSqlQueryProto::escape(const QString& str)
{
    QString _str (str);
    QVariant v = thisQuery ()->driver ()->handle();
    if (v.isValid() && qstrcmp(v.typeName(), "sqlite3*")==0)
        _str.replace ('\'', "''");

    return _str;
}

// ============================================================

jsDB::jsDB(QObject* parent)
    : QObject (parent)
{
    QString rootDir = BGMRPC::Settings->value ("rootDir", "~/.BGMR").toString ();
    SqliteDBPath = rootDir + "/DB";

    db = QSqlDatabase::database ();
}

bool jsDB::open(const QString& dbName, const QString& connectionName)
{
    if (!addDatabase ("QSQLITE", connectionName))
        return false;

    if (db.isOpen ())
        db.close ();

    db.setDatabaseName (SqliteDBPath + "/" + dbName);

    return db.open ();
}

bool jsDB::open(const QScriptValue& dbDef)
{
    QString driver = dbDef.property ("driver").toString ();
    QString dbName = dbDef.property ("name").toString ();
    QString usr = dbDef.property ("usr").toString ();
    QString pwd = dbDef.property ("pwd").toString ();
    QScriptValue jsPortVal = dbDef.property ("port");
    QString host = dbDef.property ("host").toString ();
    QString connectionName = dbDef.property ("connection").toString ();

    int port = -1;
    if (jsPortVal.isNumber ())
        port = jsPortVal.toInt32 ();

    if (driver.isEmpty ())
        driver = "QSQLITE";

    if (!addDatabase (driver, connectionName))
        return false;

    if (driver == "QMYSQL") {
        QString socket = dbDef.property ("socket").toString ();
        if (!socket.isEmpty ())
            db.setConnectOptions (QString ("UNIX_SOCKET=%1").arg (socket));

        db.setPort (port);
        db.setHostName (host);

        db.setUserName (usr);
        db.setPassword (pwd);
        db.setDatabaseName (dbName);
    }else if (driver == "QSQLITE")
        db.setDatabaseName (SqliteDBPath + "/" + dbName);

    if (port >= 0 || !host.isEmpty ())
        db.close ();

    if (db.isOpen ())
        db.close ();

    return db.open ();
}

//bool jsDB::switchDB(const QString& connectionName)
//{
//    if (connectionName.isEmpty ())
//        db = QSqlDatabase::database ();
//    else
//        db = QSqlDatabase::database (connectionName);

//    return db.isValid ();
//}

bool jsDB::isOpen() const
{
    return db.isOpen ();
}

QString jsDB::dbName() const
{
    return db.databaseName ();
}

bool jsDB::addDatabase(const QString& type,
                       const QString& connectionName)
{
    if (db.connectionName ().isEmpty () || db.connectionName () != connectionName)
    {
        if (connectionName.isEmpty ())
            db = QSqlDatabase::addDatabase (type);
        else
            db = QSqlDatabase::addDatabase (type, connectionName);
    }

    return db.isValid ();
}

QString jsDB::connectionName() const
{
    return db.connectionName ();
}

QStringList jsDB::connectionNames() const
{
    return QSqlDatabase::connectionNames ();
}

QString jsDB::hostName() const
{
    return db.hostName ();
}

QSqlQuery jsDB::newQuery() const
{
    return QSqlQuery (db);
}

//void jsDB::setQSqliteDBPath(const QString& path)
//{
//    SqliteDBPath = path;
//}
