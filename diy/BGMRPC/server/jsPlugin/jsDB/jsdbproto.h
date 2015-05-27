#ifndef JSDBPROTO_H
#define JSDBPROTO_H

#include <QObject>
#include <bgmrpc.h>
#include <QtSql/QtSql>
#include <qsqldatabase.h>

#include "jsobjectclass.h"

using namespace BGMircroRPCServer;

class jsSqlQueryProto;
template <>
class protoTypeInfo < jsSqlQueryProto >
{
public:
    typedef QSqlQuery dataType;
    static QString className () { return "SQLQuery"; }
    static bool isNull (dataType) { return false; }
    static QSqlQuery nullData () { return QSqlQuery (); }
    static dataType newObject () { return QSqlQuery (); }
};

class jsSqlQueryProto : public QObject, public QScriptable
{
    Q_PROPERTY(QString lastError READ lastError)
    Q_PROPERTY(QScriptValue record READ record)
    Q_PROPERTY(QString lastQuery READ lastQuery)

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
    Q_INVOKABLE bool deleteRecord (const QString& table,
                                   const QScriptValue& rec);

    Q_INVOKABLE bool next ();
    Q_INVOKABLE bool previous ();
    Q_INVOKABLE bool first();
    Q_INVOKABLE bool last ();
    Q_INVOKABLE bool seek (int index);
    Q_INVOKABLE QScriptValue record (const QScriptValue& _query = QScriptValue (QScriptValue::UndefinedValue)) const;
    Q_INVOKABLE QString escape (const QString& str);

    Q_INVOKABLE QString lastError () const;
    Q_INVOKABLE QString lastQuery () const;

private:
    QScriptValue _record (QSqlQuery* query = NULL) const;
    QSqlQuery* thisQuery () const;
    QString toDBValue(const QScriptValue& fv);
    QString whereStatem (const QScriptValue& rec);
};

typedef jsObjectClass < jsSqlQueryProto > jsSqlQueryClass;
Q_DECLARE_METATYPE (QSqlQuery)
Q_DECLARE_METATYPE (QSqlQuery*)
Q_DECLARE_METATYPE (jsSqlQueryClass*)

class jsDB : public QObject {
    Q_PROPERTY(QString dbName READ dbName)
    Q_PROPERTY(QString connectionName READ connectionName)
    Q_PROPERTY(QString hostName READ hostName)
    Q_PROPERTY(QStringList connectionNames READ connectionNames)

    Q_OBJECT

public:
    jsDB (BGMRObjectInterface* obj, QObject* parent = 0);

    Q_INVOKABLE bool open (const QScriptValue& dbDef, bool isConnect = false);
    Q_INVOKABLE void close();
    Q_INVOKABLE bool isOpen () const;
    Q_INVOKABLE QString dbName () const;
    Q_INVOKABLE QString connectionName () const;
    Q_INVOKABLE QStringList connectionNames () const;
    Q_INVOKABLE QString hostName () const;
    Q_INVOKABLE QSqlQuery newQuery () const;
    Q_INVOKABLE bool lockDatabase (const QString& pwd);
    Q_INVOKABLE bool lockDatabaseByObject ();
    Q_INVOKABLE bool unlockDatabase ();

private:
    bool checkAuth (const QSqlDatabase& db, const QString& usr,
                    const QString& pwd);
    bool initialAdminSqliteDatabase(QSqlDatabase& adminDB);
    QSqlDatabase openSqliteAdminDatabase ();
    QSqlDatabase DB;
    QString SqliteDBPath;
    BGMRObjectInterface* JsObj;
};

#endif // JSDBPROTO_H
