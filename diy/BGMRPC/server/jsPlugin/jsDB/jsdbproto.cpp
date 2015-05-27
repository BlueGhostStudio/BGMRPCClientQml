#include "jsdbproto.h"

#include <bgmrobject.h>

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
            jsResult.setProperty (index, _record (query));
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

    qDebug () << qstr;

    return thisQuery ()->exec (qstr);
}

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

QScriptValue jsSqlQueryProto::record(/*QSqlQuery**/const QScriptValue& _query) const
{
    return _record (engine ()->fromScriptValue < QSqlQuery* > (_query));
}

QString jsSqlQueryProto::lastError() const
{
    return thisQuery ()->lastError ().text ();
}

QString jsSqlQueryProto::lastQuery() const
{
    return thisQuery ()->lastQuery ();
}

QScriptValue jsSqlQueryProto::_record(QSqlQuery* query) const
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

QSqlQuery* jsSqlQueryProto::thisQuery() const
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

jsDB::jsDB(BGMRObjectInterface* obj, QObject* parent)
    : QObject (parent), JsObj (obj)
{
    QString rootDir = BGMRPC::Settings->value ("rootDir", "~/.BGMRPC").toString ();
    SqliteDBPath = rootDir + "/DB";
}

bool jsDB::open(const QScriptValue& dbDef, bool isConnect)
{
    QString dbName = dbDef.property ("dbName").toString ();
    if (dbName.isEmpty ())
        return false;

    QString driver = dbDef.property ("driver").toString ();
    QString usr = dbDef.property ("usr").toString ();
    QString pwd = dbDef.property ("pwd").toString ();
    QScriptValue jsPortVal = dbDef.property ("port");
    QString host = dbDef.property ("host").toString ();
    QString connName = dbDef.property ("connectName").toString ();

    int port = -1;
    if (jsPortVal.isNumber ())
        port = jsPortVal.toInt32 ();

    if (driver.isEmpty ())
        driver = "QSQLITE";

    QString connectBaseName = QString ("%1_%2_%3")
                              .arg (JsObj->objectName ())
                              .arg (driver)
                              .arg (dbName);
    if (connName.isEmpty ())
        connName = connectBaseName;
    else if (!isConnect)
        connName = connectBaseName + '_' + connName;

    bool ok = false;
    QSqlDatabase theDB;
    if (QSqlDatabase::contains (connName)) {
        theDB = QSqlDatabase::database (connName);
        QString openedDBName = theDB.databaseName ();
        QString openedDBName_sqlite = (driver == "QSQLITE")
                                      ? QFileInfo (openedDBName).fileName ()
                                      : "";

        if (theDB.driverName () == driver
                && (openedDBName == dbName || openedDBName_sqlite == dbName))
            ok = checkAuth (theDB, usr, pwd);
    } else if (!isConnect) {
        theDB = QSqlDatabase::addDatabase (driver, connName);
        if (driver == "QMYSQL") {
            QString socket = dbDef.property ("socket").toString ();
            if (!socket.isEmpty ())
                theDB.setConnectOptions (QString ("UNIX_SOCKET=%1").arg (socket));
            if (port >= 0)
                theDB.setPort (port);
            if (!host.isEmpty ())
                theDB.setHostName (host);
            theDB.setUserName (usr);
            theDB.setPassword (pwd);
            theDB.setDatabaseName (dbName);
            ok = theDB.open ();
        } else if (driver == "QSQLITE") {
            theDB.setDatabaseName (SqliteDBPath + "/" + dbName);
            if (checkAuth (theDB, usr, pwd))
                ok = theDB.open ();
            else
                ok = false;
        }

        if (!ok)
            QSqlDatabase::removeDatabase (connName);
    }

    if (ok)
        DB = theDB;

    return ok;
}

void jsDB::close()
{
    if (DB.isOpen ())
        DB.close ();
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
    return DB.isOpen ();
}

QString jsDB::dbName() const
{
    QString name;
    if (DB.isOpen ())
        name = QFileInfo (DB.databaseName ()).fileName ();

    return name;
}

QString jsDB::connectionName() const
{
    QString name;
    if (DB.isOpen ())
        name = DB.connectionName ();

    return name;
}

QStringList jsDB::connectionNames() const
{
    return QSqlDatabase::connectionNames ();
}

QString jsDB::hostName() const
{
    QString name;
    if (DB.isOpen ())
        name = DB.hostName ();

    return name;
}

QSqlQuery jsDB::newQuery() const
{
    if (DB.isOpen ())
        return QSqlQuery (DB);
    else
        return QSqlQuery ();
}

bool jsDB::lockDatabase(const QString& pwd)
{
    bool ok = false;

    if (DB.isOpen () && DB.driverName () == "QSQLITE") {
        QSqlDatabase adminDB = openSqliteAdminDatabase ();
        if (adminDB.isOpen ()) {
            QString dbName = DB.databaseName ();
            dbName = QFileInfo (dbName).fileName ();

            QSqlQuery adminQuery (adminDB);
            adminQuery.prepare ("SELECT * FROM database "
                                "WHERE dbname=:dbName");
            adminQuery.bindValue (":dbName", dbName);
            adminQuery.exec ();

            if (adminQuery.next ())
                adminQuery.prepare ("UPDATE database SET "
                                    "pwd=:pwd WHERE dbname=:dbName");
            else
                adminQuery.prepare ("INSERT INTO database (dbname,pwd) "
                                    "VALUES (:dbName, :pwd)");

            adminQuery.bindValue (":dbName", dbName);
            adminQuery.bindValue (":pwd", pwd);
            ok = adminQuery.exec ();
        }
    }

    return ok;
}

bool jsDB::lockDatabaseByObject()
{
    bool ok = false;

    if (DB.isOpen () && DB.driverName () == "QSQLITE") {
        QSqlDatabase adminDB = openSqliteAdminDatabase ();
        if (adminDB.isOpen ()) {
            QString dbName = DB.databaseName ();
            dbName = QFileInfo (dbName).fileName ();

            QSqlQuery adminQuery (adminDB);
            adminQuery.prepare ("SELECT * FROM database "
                                "WHERE dbname=:dbName");
            adminQuery.bindValue (":dbName", dbName);
            adminQuery.exec ();

            if (adminQuery.next ())
                adminQuery.prepare ("UPDATE database SET "
                                    "obj=:obj WHERE dbname=:dbName");
            else
                adminQuery.prepare ("INSERT INTO database (dbname,obj) "
                                    "VALUES (:dbName, :obj)");
            adminQuery.bindValue (":dbName", dbName);
            adminQuery.bindValue (":obj", JsObj->objectName ());

            ok = adminQuery.exec ();
        }
    }

    return ok;
}

bool jsDB::unlockDatabase()
{
    bool ok = false;

    if (DB.isOpen () && DB.driverName () == "QSQLITE") {
        QSqlDatabase adminDB = openSqliteAdminDatabase ();
        if (adminDB.isOpen ()) {
            QString dbName = DB.databaseName ();
            dbName = QFileInfo (dbName).fileName ();

            QSqlQuery adminQuery (adminDB);
            adminQuery.prepare ("DELETE FROM database "
                                "WHERE dbname=:dbName");
            adminQuery.bindValue (":dbName", dbName);
            ok = adminQuery.exec ();
        }
    }

    return ok;
}

bool jsDB::checkAuth(const QSqlDatabase& db, const QString& usr,
                     const QString& pwd)
{
    bool ok = false;
    QString driver = db.driverName ();
    if (db.driverName () == "QMYSQL")
        ok = db.userName () == usr && db.password () == pwd;
    else if (driver == "QSQLITE"){
        QSqlDatabase adminDB = openSqliteAdminDatabase ();

        if (adminDB.isOpen ()) {
            QString dbName = db.databaseName ();
            dbName = QFileInfo (dbName).fileName ();
            QSqlQuery adminQuery (adminDB);

            adminQuery.prepare ("SELECT * FROM database "
                                "WHERE dbname=:dbName");
            adminQuery.bindValue (":dbName", dbName);
            adminQuery.exec ();

            if (adminQuery.next ()) {
                QString db_obj = adminQuery.value ("obj").toString ();
                QString db_pwd = adminQuery.value ("pwd").toString ();
                if (!db_obj.isEmpty ())
                    ok = db_obj == JsObj->objectName ();
                else
                    ok = db_pwd == pwd;
            } else
                ok = true;

            if (adminQuery.size () > 0)
                ok = true;
        }
    }

    return ok;
}

bool jsDB::initialAdminSqliteDatabase(QSqlDatabase& adminDB)
{
    QSqlQuery adminQuery (adminDB);

    return adminQuery.exec ("CREATE TABLE database "
                            "(dbname VARCHAR(20) PRIMARY KEY, "
                            "pwd VARCHAR(20), obj VARCHAR(20))");
}

QSqlDatabase jsDB::openSqliteAdminDatabase()
{
    QSqlDatabase adminDB;
    if (!QSqlDatabase::contains ("admin")) {
        adminDB = QSqlDatabase::addDatabase ("QSQLITE", "admin");
        adminDB.setDatabaseName (SqliteDBPath + "/admin");
        adminDB.open ();
        if (QFile::exists (SqliteDBPath + "/admin"))
            initialAdminSqliteDatabase (adminDB);
    } else
        adminDB = QSqlDatabase::database ("admin");

    return adminDB;
}
