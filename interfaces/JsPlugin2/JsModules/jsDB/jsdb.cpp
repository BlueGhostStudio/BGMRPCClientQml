#include "jsdb.h"

#include <QDebug>
#include <QJSValueIterator>

JsDB::JsDB(QObject* parent) : QObject(parent) {}

bool
JsDB::openDB(const QString& dbName, QString connectionName) {
    if (dbName.isEmpty()) return false;

    QJSEngine* engine = qjsEngine(this);
    QJSValue JS = engine->globalObject().property("JS");

    if (connectionName.isEmpty()) {
        connectionName = QString("jsDB_%1_%2")
                             .arg(JS.property("__NAME__").toString())
                             .arg(dbName);
    }

    if (QSqlDatabase::contains(connectionName))
        DB = QSqlDatabase::database(connectionName, false);
    else
        DB = QSqlDatabase::addDatabase("QSQLITE", connectionName);

    if (!DB.isOpen()) {
        DB.setDatabaseName(JS.property("__PATH_DATA__").toString() + '/' +
                           dbName);
        return DB.open();
    }

    return true;
}

bool
JsDB::connectDB(const QString connectionName) {
    DB = QSqlDatabase::database(connectionName, false);

    return DB.isOpen();
}

QJSValue
JsDB::exec(const QString& stm, const QJSValue& bind) const {
    //    QJsonObject ret;
    QVariantMap ret;
    QSqlQuery query(DB);
    query.prepare(stm);

    /*QJsonObject::const_iterator it;
    for (it = bind.constBegin(); it != bind.constEnd(); ++it) {
        QVariant binVal;
        if (it.value().type() == QJsonValue::Double)
            binVal = QVariant(it.value().toDouble());
        else
            binVal = QVariant(it.value().toString());
        query.bindValue(it.key(), binVal);
    }*/
    if (bind.isObject()) {
        QJSValueIterator it(bind);
        while (it.hasNext()) {
            it.next();
            query.bindValue(it.name(), it.value().toVariant());
        }
    }

    if (query.exec()) {
        ret["ok"] = true;

        if (query.isSelect()) {
            QVariantList rows;
            //            QJsonArray rows;
            while (query.next()) {
                //                QJsonObject row;
                QVariantMap row;
                QSqlRecord record = query.record();
                for (int i = 0; i < record.count(); i++) {
                    // row[record.field (i).name ()] = record.value (i);
                    QString fieldName = record.field(i).name();
                    QVariant data = record.value(i);
                    row[fieldName] = data;
                    /*if ((QMetaType::Type)data.type() == QMetaType::QByteArray)
                    { qDebug()
                            << "---------------" << data.toByteArray().length();
                        row[fieldName] = QJsonValue(
                            QString::fromUtf8(data.toByteArray().constData(),
                                              data.toByteArray().length()));
                    } else
                        row[fieldName] = QJsonValue::fromVariant(data);*/
                    /*row[record.field(i).name()] =
                        QJsonValue::fromVariant(record.value(i));*/
                }
                rows.append(row);
            }
            ret["rows"] = rows;
        } else
            ret["lastInsert"] = query.lastInsertId().toInt();
    } else {
        ret["ok"] = false;
        ret["error"] = query.lastError().text();
    }

    return qjsEngine(this)->toScriptValue(ret);
}

JsDBFactory::JsDBFactory(QObject* parent) : JsObjFactory(parent, false) {}

QObject*
JsDBFactory::constructor(const QVariant&) const {
    return new JsDB();
}

void
initialModule(QJSEngine* engine) {
    registerJsType<JsDBFactory>("JsDB", *engine);
}
