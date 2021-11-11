#ifndef JSDB_H
#define JSDB_H

#include <QJSEngine>
#include <QObject>
#include <QtSql>

#include "jsdb_global.h"
#include "jsobjfactory.h"

class JsDB : public QObject {
    Q_OBJECT

public:
    JsDB(QObject* parent = nullptr);

    Q_INVOKABLE bool openDB(const QString& dbName,
                            QString connectionName = QString());
    Q_INVOKABLE bool connectDB(const QString connectionName);
    Q_INVOKABLE QJSValue exec(const QString& stm,
                              const QJSValue& bind = QJSValue()) const;

private:
    QSqlDatabase DB;
};

class JsDBFactory : public JsObjFactory {
    Q_OBJECT

public:
    JsDBFactory(QObject* parent = nullptr);

protected:
    QObject* constructor(const QVariant&) const;
};

extern "C" {
void initialModule(QJSEngine* engine);
}

#endif  // JSDB_H
