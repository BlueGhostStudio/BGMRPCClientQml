#ifndef JSDB_H
#define JSDB_H

#include <QObject>
#include <QJSEngine>
#include <QtSql>
#include "jsobjfactory.h"

#include "jsdb_global.h"

class JsDB : public QObject
{
    Q_OBJECT

public:
    JsDB (QObject* parent = nullptr);

    Q_INVOKABLE bool openDB(const QString& dbName,
                            QString connectionName = QString ());
    Q_INVOKABLE bool connectDB(const QString connectionName);
    Q_INVOKABLE QJsonObject exec(const QString& stm,
                                const QJsonObject& bind = QJsonObject ()) const;

private:
    QSqlDatabase DB;
};

class JsDBFactory : public JsObjFactory {
    Q_OBJECT

public:
    JsDBFactory (QObject* parent = nullptr);

protected:
    QObject* constructor () const;
};

extern "C" {
void initialModule (QJSEngine* engine);
}

#endif // JSDB_H
