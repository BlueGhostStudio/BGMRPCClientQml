#ifndef JSOBJECTPROTOTYPE_H
#define JSOBJECTPROTOTYPE_H

#include <QObject>
#include <QtScript>
#include <bgmrprocedure.h>
#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <bgmrobjectstorage.h>
#include <QtSql/QtSql>
#include <qsqldatabase.h>

#include "jsmetatype.h"
#include "jsobjectclass.h"

using namespace BGMircroRPCServer;

typedef BGMRProcedure* procPtr;

class jsProcProto : public QObject, public QScriptable
{
    Q_PROPERTY( qulonglong pID READ pID )
    Q_PROPERTY(bool isKeepConnected READ isKeepConnected)

Q_OBJECT
public:
    jsProcProto (QObject* parent = 0);

    qulonglong pID ();

    Q_INVOKABLE void emitSignal (BGMRObjectInterface* obj,
                                 const QString& signal,
                                 const QJsonArray& args);
//    Q_INVOKABLE void setPrivateData (const QString& key,
//                                     const QJsonValue& value);
//    Q_INVOKABLE QJsonValue privateData (const QString& key) const;
    Q_INVOKABLE void setPrivateData (BGMRObjectInterface* obj,
                                     const QString& key,
                                     const QJsonValue& value);
    Q_INVOKABLE QJsonValue privateData (BGMRObjectInterface* obj,
                                        const QString& key) const;
    Q_INVOKABLE bool isKeepConnected () const;
    Q_INVOKABLE QJsonArray callMethod (const QString& obj,
                                       const QString& method,
                                       const QJsonArray& args);
    Q_INVOKABLE void close ();

private:
    procPtr thisProc() const;
};

template <>
class protoTypeInfo < jsProcProto >
{
public:
    typedef procPtr dataType;
    static QString className () { return "BGMRProcedure"; }
};

typedef jsObjectClass < jsProcProto > jsProcClass;

Q_DECLARE_METATYPE (const BGMRProcedure*)
Q_DECLARE_METATYPE (BGMRProcedure*)
Q_DECLARE_METATYPE (jsProcClass*)

// ===========================================================

class jsRPCObjectProto;
typedef BGMRObjectInterface* rpcObjPtr;

template <>
class protoTypeInfo < jsRPCObjectProto >
{
public:
    typedef rpcObjPtr dataType;
    static QString className () { return "BGMRObject"; }
};

class jsRPCObjectProto : public QObject, public QScriptable
{
    Q_PROPERTY(QString __NAME__ READ objectName)
    Q_PROPERTY(QString __TYPE__ READ objectType)
    Q_PROPERTY(QStringList __METHODS__ READ objectMethods)
    Q_OBJECT
public:
    jsRPCObjectProto (QObject* parent = 0);

    Q_INVOKABLE QString objectName () const;
    Q_INVOKABLE QString objectType () const;
    Q_INVOKABLE QStringList objectMethods () const;
    Q_INVOKABLE QJsonArray callMethod (BGMRProcedure* proc,
                                       const QString& method,
                                       const QJsonArray& args);

protected:
    virtual BGMRObjectInterface* thisRPCObj () const;
};

typedef jsObjectClass < jsRPCObjectProto > jsRPCObjectClass;
Q_DECLARE_METATYPE (const BGMRObjectInterface*)
Q_DECLARE_METATYPE (BGMRObjectInterface*)
Q_DECLARE_METATYPE (jsRPCObjectClass*)

// ===========================================================

class jsObj;
class jsJsObjProto;
typedef jsObj* jsObjPtr;

template <>
class protoTypeInfo < jsJsObjProto >
{
public:
    typedef jsObjPtr dataType;
    static QString className () { return "jsObj"; }
};

class jsJsObjProto : public jsRPCObjectProto
{
    Q_PROPERTY(relProcsMap relProcs READ relProcs)
    Q_OBJECT
public:
    jsJsObjProto(QObject* parent = 0);

    Q_INVOKABLE void addProc (BGMRProcedure* proc);
    Q_INVOKABLE bool removeProc (qulonglong pID);
    Q_INVOKABLE relProcsMap relProcs () const;
    Q_INVOKABLE BGMRProcedure* relProc (qulonglong pID) const;
    Q_INVOKABLE void onRelProcDisconnected (const QScriptValue& handel);
    Q_INVOKABLE bool containsRelProc (qulonglong pID) const;
    Q_INVOKABLE void emitSignal (const QString& signal,
                                 const QJsonArray& args) const;

protected:
    BGMRObjectInterface* thisRPCObj () const;
    jsObjPtr thisJsObj () const;
};

typedef jsObjectClass < jsJsObjProto > jsJsObjClass;
Q_DECLARE_METATYPE (jsJsObjClass*)


// ===========================================================

class jsRPC : public QObject
{
    Q_PROPERTY(RPCObjList objects READ objects)
    Q_PROPERTY(QStringList types READ types)
    Q_OBJECT
public:
    jsRPC (QObject* parent = 0);

    Q_INVOKABLE RPCObjList objects () const;
    Q_INVOKABLE QStringList types () const;
    Q_INVOKABLE bool creatorObject (const QString& type, const QString& objName);
    Q_INVOKABLE bool installPlugin (const QString& pluginFileName);
    void setRPC (BGMRPC* rpc);

private:
    BGMRPC* RPC;
};

// ============================================================

class jsSqlQueryProto;
template <>
class protoTypeInfo < jsSqlQueryProto >
{
public:
    typedef QSqlQuery dataType;
    static QString className () { return "SQLQuery"; }
};

// DB.open ("myDB");
// var q = new SQL ();
// q.query ("select * from artist");
// var result = q.fetch ("select * from artist");
// DB.insert (artist, {name:"BG", age:23});
// DB.update (artist, "id=0", {name:"atomCat", arg:34});
// var q = DB.query ("select * from artist");
// var result = DB.fetch ("select * from artist");
// DB.open ("myDB1", "dev");
// q.query ("select * ...", "dev");
// DB.query ("...", "dev");
// [["field1","type(len)",1]["field2","type(len)"]]
//
class jsSqlQueryProto : public QObject, public QScriptable
{
    Q_PROPERTY(QString lastError READ lastError)
    Q_PROPERTY(QScriptValue record READ record)

    Q_OBJECT
public:
    jsSqlQueryProto (QObject* parent = 0);

    Q_INVOKABLE void close();

    Q_INVOKABLE bool exec (const QString& q);
    Q_INVOKABLE QScriptValue fetch (const QString& q);
    Q_INVOKABLE bool insertRecord(const QString& table,
                                  const QScriptValue& jsRec);
    Q_INVOKABLE bool updateRecord (const QString& talbe,
                                   const QScriptValue& oldRec,
                                   const QScriptValue& newRec);
//    Q_INVOKABLE bool updateRecord (const QString& table,
//                                   const QString& conditions,
//                                   const QScriptValue& rec){}
//    Q_INVOKABLE bool deleteRecord (const QString& table,
//                                   const QString& conditions);
    Q_INVOKABLE bool deleteRecord (const QString& table,
                                   const QScriptValue& rec);

    Q_INVOKABLE bool next ();
    Q_INVOKABLE bool previous ();
    Q_INVOKABLE bool first();
    Q_INVOKABLE bool last ();
    Q_INVOKABLE bool seek (int index);
    Q_INVOKABLE QScriptValue record (QSqlQuery* query = NULL) const;
    Q_INVOKABLE QString escape (const QString& str);

    Q_INVOKABLE QString lastError () const;

private:
    QSqlQuery* thisQuery () const;
    QString toDBValue(const QScriptValue& fv);
    QString whereStatem (const QScriptValue& rec);
};

typedef jsObjectClass < jsSqlQueryProto > jsSqlQueryClass;
Q_DECLARE_METATYPE (QSqlQuery)
Q_DECLARE_METATYPE (QSqlQuery*)
Q_DECLARE_METATYPE (jsSqlQueryClass*)

// ============================================================

class jsDB : public QObject {
    Q_PROPERTY(QString dbName READ dbName)
    Q_PROPERTY(QString connectionName READ connectionName)
    Q_PROPERTY(QString hostName READ hostName)
    Q_PROPERTY(QStringList connectionNames READ connectionNames)
    Q_OBJECT
public:
    jsDB (jsObj* obj, QObject* parent = 0);

    Q_INVOKABLE bool open (const QScriptValue& dbDef);
    Q_INVOKABLE void close();
//    Q_INVOKABLE bool switchDB (const QString& connectionName = QString ());
    Q_INVOKABLE bool isOpen () const;
    Q_INVOKABLE QString dbName () const;
    Q_INVOKABLE QString connectionName () const;
    Q_INVOKABLE QStringList connectionNames () const;
    Q_INVOKABLE QString hostName () const;
    Q_INVOKABLE QSqlQuery newQuery () const;
    Q_INVOKABLE bool lockDatabase (const QString& pwd);
    Q_INVOKABLE bool lockDatabaseByObject ();
    Q_INVOKABLE bool unlockDatabase ();

//    void setQSqliteDBPath (const QString& path);

private:
    bool checkAuth (const QSqlDatabase& db, const QString& usr,
                    const QString& pwd);
    bool initialAdminSqliteDatabase(QSqlDatabase& adminDB);
    QSqlDatabase openSqliteAdminDatabase ();
    QSqlDatabase DB;
    QString SqliteDBPath;
    jsObj* JsObj;
};

#endif // JSOBJECTPROTOTYPE_H
