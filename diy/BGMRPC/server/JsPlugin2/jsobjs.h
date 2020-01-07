#ifndef JSOBJS_H
#define JSOBJS_H

#include <QObject>
#include <bgmrobject.h>
#include <bgmrclient.h>
#include <QJSEngine>
#include <bgmrpc.h>

#include "jsengine.h"

using namespace BGMircroRPCServer;

class JsRpcObj;
class JsJSObj;

class JsClient : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qlonglong cliID READ cliID)
    Q_PROPERTY(bool online READ online)
public:
    explicit JsClient(BGMRClient* cli, QObject *parent = nullptr);

    qulonglong cliID() const;
    bool online() const;
    Q_INVOKABLE QJSValue clone() const;
    Q_INVOKABLE void setPrivateData(const QJSValue& obj, const QString& key,
                                    const QJsonValue& value);
    Q_INVOKABLE QJsonValue privateData(const QJSValue& obj,
                                       const QString& key) const;

    /*Q_INVOKABLE QJsonValue callMethod (const QJSValue& obj,
                                       const QString& method,
                                       const QJsonArray& args) const;*/
    Q_INVOKABLE void emitSignal(const QJSValue& obj, const QString& signal,
                                const QJsonArray& args) const;

    Q_INVOKABLE void close();

    BGMRClient* client();

signals:

public slots:

protected:
    BGMRClient* Cli;
};

class JsRpcObj : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString __NAME__ READ objectName)
    Q_PROPERTY(QString __TYPE__ READ objectType)
    Q_PROPERTY(QStringList __METHODS__ READ objectMethods)

public:
    explicit  JsRpcObj(BGMRObjectInterface* obj, QObject* parent = nullptr);
    BGMRObjectInterface* rpcObj() const;

    QString objectName() const;
    QString objectType() const;
    QStringList objectMethods() const;

    Q_INVOKABLE QJsonArray callMethod(const QJSValue& cli,
                                      const QString& method,
                                      const QJsonArray& args);

protected:
    BGMRObjectInterface* RpcObj;
};

class JsJSObj : public JsRpcObj
{
    Q_OBJECT

public:
    explicit JsJSObj(BGMRObjectInterface* obj, QObject* parent = nullptr);

    Q_INVOKABLE QString objPath() const;

    Q_INVOKABLE void setGlobalMutexLock(bool lock) const;
    Q_INVOKABLE bool globalMutexLock() const;
    Q_INVOKABLE void mutexLock() const;
    Q_INVOKABLE void mutexUnlock() const;

    Q_INVOKABLE void addRelClient(const QJSValue& cli) const;
    Q_INVOKABLE bool removeClient(const QJSValue& cli) const;
    Q_INVOKABLE QJSValue relClients(bool autoDel = true) const;
    Q_INVOKABLE QJSValue relClient(int cliID, bool autoDel = true) const;
    Q_INVOKABLE void onRelClientRemoved(const QJSValue&handle);
    Q_INVOKABLE bool containsRelClient(const QJSValue& cli) const;
    Q_INVOKABLE QJSValue findRelClient(QJSValue callback, bool autoDel = true) const;

    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QJsonArray& args) const;

    Q_INVOKABLE bool include(const QString& scrFileName) const;
    Q_INVOKABLE bool loadScript(const QString& scrFileName,
                                const QJSValue& arg0 = QJSValue(),
                                const QJSValue& arg1 = QJSValue(),
                                const QJSValue& arg2 = QJSValue(),
                                const QJSValue& arg3 = QJSValue(),
                                const QJSValue& arg4 = QJSValue(),
                                const QJSValue& arg5 = QJSValue(),
                                const QJSValue& arg6 = QJSValue(),
                                const QJSValue& arg7 = QJSValue(),
                                const QJSValue& arg8 = QJSValue(),
                                const QJSValue& arg9 = QJSValue()) const;
    Q_INVOKABLE void loadModule(const QString& module) const;

    Q_INVOKABLE QJSValue toJS(const QJSValue& obj, bool autoDel = true) const;

private:
    JsEngine* toJsEngine() const;
    QJSValue RelClientRemovedHandle;
};

class JsRPC : public QObject
{
    Q_OBJECT

public:
    JsRPC(BGMRPC* rpc, QObject* parent = nullptr);

    Q_INVOKABLE QJSValue object(const QString& objName,
                                bool autoDel = true) const;
    Q_INVOKABLE QJSValue objects(bool autoDel = true) const;
    Q_INVOKABLE QStringList types() const;
    Q_INVOKABLE QJSValue createObject(const QString& objType,
                                      const QString& objName,
                                      bool autoDel = true) const;
    Q_INVOKABLE bool removeObject(const QJSValue& obj) const;
    Q_INVOKABLE bool installPlugin(const QString& pluginFileName) const;

private:
    BGMRPC* RPC;
};

#endif // JSPROC_H
