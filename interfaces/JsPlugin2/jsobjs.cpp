#include "jsobjs.h"
#include <QFile>
#include <QFileInfo>

using namespace NS_BGMRPCObjectInterface;

/*ObjectInterface* toRPCObj(const QJSValue& obj)
{
    return qobject_cast<JsRpcObj*>(obj.toQObject())->rpcObj();
}*/
// =================

JsCaller::JsCaller(QPointer<Caller> caller, QObject* parent)
    : QObject(parent), m_caller(caller)
{
}

quint64 JsCaller::cliID() const
{
    if (m_caller.isNull())
        return 0;
    else
        return m_caller->ID();
}

bool JsCaller::online() const
{
    if (m_caller.isNull())
        return false;
    else
        return m_caller->exited();
}

QJSValue JsCaller::clone() const
{
    if (m_caller.isNull())
        return QJSValue();
    else
        return qjsEngine(this)->newQObject(new JsCaller(m_caller));
}

void JsCaller::setPrivateData(const QJSValue& obj, const QString& key,
                              const QJSValue& value)
{
    JsJSObj* theObj = qobject_cast<JsJSObj*>(obj.toQObject());
    if (theObj && !m_caller.isNull())
        theObj->setPrivateData(m_caller, key, value);
}

QJSValue JsCaller::privateData(const QJSValue& obj, const QString& key)
{
    JsJSObj* theObj = qobject_cast<JsJSObj*>(obj.toQObject());
    if (theObj && !m_caller.isNull())
        return theObj->privateData(m_caller, key);
    else
        return QJSValue();
}

void JsCaller::emitSignal(const QJSValue& /*obj*/, const QString& signal,
                          const QJSValue& args) const
{
    emitSignal(signal, args);
}

void JsCaller::emitSignal(const QString& signal, const QJSValue& args) const
{
    if (!m_caller.isNull())
        m_caller->emitSignal(signal, args.toVariant());
}

QPointer<Caller> JsCaller::caller() const
{
    return m_caller;
}

JsJSObj::JsJSObj(JsEngine* jsEngine, QObject* parent)
    : QObject(parent), m_jsEngine(jsEngine)
{
}

QString JsJSObj::objectName() const
{
    return m_jsEngine->objectName();
}

JsEngine* JsJSObj::jsEngine() const
{
    return m_jsEngine;
}

void JsJSObj::setPrivateData(const QPointer<Caller>& caller, const QString& key,
                             const QJSValue& value)
{
    m_jsEngine->setPrivateData(caller, key, value.toVariant());
}

QJSValue JsJSObj::privateData(QPointer<Caller> caller, const QString& key) const
{
    return qjsEngine(this)->toScriptValue(m_jsEngine->privateData(caller, key));
}

void JsJSObj::setPrivateData(const QJSValue& caller, const QString& key,
                             const QJSValue& value)
{
    JsCaller* theJsCaller = qobject_cast<JsCaller*>(caller.toQObject());
    setPrivateData(theJsCaller->caller(), key, value);
}

QJSValue JsJSObj::privateData(const QJSValue& caller, const QString& key) const
{
    JsCaller* theJsCaller = qobject_cast<JsCaller*>(caller.toQObject());
    return privateData(theJsCaller->caller(), key);
}

bool JsJSObj::addRelClient(const QJSValue& caller) const
{
    JsCaller* theJsCaller = qobject_cast<JsCaller*>(caller.toQObject());
    if (theJsCaller && theJsCaller->online()) {
        m_jsEngine->addRelatedCaller(theJsCaller->caller());
        return true;
    } else
        return false;
}

bool JsJSObj::removeRelClient(const QJSValue& caller) const
{
    JsCaller* theJsCaller = qobject_cast<JsCaller*>(caller.toQObject());
    if (theJsCaller && theJsCaller->online())
        return m_jsEngine->removeRelatedCaller(theJsCaller->caller());
    else
        return false;
}

QJSValue JsJSObj::relClients(bool autoDel) const
{
    QJSEngine* engine = qjsEngine(this);
    QJSValue ret = engine->newArray();

    int i = 0;
    m_jsEngine->findRelatedCaller([=, &ret](QPointer<Caller> caller) -> bool {
        JsCaller* retItem = new JsCaller(caller);
        if (autoDel)
            retItem->deleteLater();
        ret.setProperty(i, engine->newQObject(retItem));

        return false;
    });

    return ret;
}

QJSValue JsJSObj::relClient(int callerID, bool autoDel) const
{
    QJSEngine* engine = qjsEngine(this);

    QPointer<Caller> caller =
        m_jsEngine->findRelatedCaller([&](QPointer<Caller> caller) -> bool {
            if (caller->ID() == callerID)
                return true;
            else
                return false;
        });

    if (!caller.isNull()) {
        JsCaller* jsCaller = new JsCaller(caller);
        if (autoDel)
            jsCaller->deleteLater();

        return engine->newQObject(jsCaller);
    }

    return QJSValue();
}

bool JsJSObj::containsRelClient(const QJSValue& caller) const
{
    JsCaller* target = qobject_cast<JsCaller*>(caller.toQObject());

    QPointer<Caller> found =
        m_jsEngine->findRelatedCaller([=](QPointer<Caller> caller) -> bool {
            if (caller->ID() == target->caller()->ID())
                return true;
            else
                return false;
        });

    return !found.isNull();
}

QJSValue JsJSObj::findRelClient(QJSValue& callback, bool autoDel) const
{
    if (!callback.isCallable())
        return QJSValue();

    QJSEngine* engine = qjsEngine(this);

    QPointer<Caller> found =
        m_jsEngine->findRelatedCaller([&](QPointer<Caller> caller) -> bool {
            JsCaller* jsCaller = new JsCaller(caller);
            jsCaller->deleteLater();

            return callback.call(QJSValueList() << engine->newQObject(jsCaller))
                .toBool();
        });

    if (found.isNull())
        return QJSValue();
    else {
        JsCaller* jsFound = new JsCaller(found);
        if (autoDel)
            jsFound->deleteLater();
        return engine->newQObject(jsFound);
    }
}

void JsJSObj::onRelClientRemoved(const QJSValue& handle)
{
    m_relClientRemovedHandle = handle;
    if (m_relClientRemovedHandle.isCallable()) {
        QObject::connect(m_jsEngine, &JsEngine::relatedCallerExited,
                         [=](QPointer<Caller> caller) {
                             JsCaller* _client_ = new JsCaller(caller);
                             _client_->deleteLater();
                             m_relClientRemovedHandle.call(
                                 QJSValueList()
                                 << qjsEngine(this)->newQObject(_client_));
                         });
    }
}

void JsJSObj::emitSignal(const QString& signal, const QJsonArray& args) const
{
    m_jsEngine->emitSignal(signal, args.toVariantList());
}

bool JsJSObj::include(const QString& scrFileName) const
{
    QString scrPath;
    if (QFileInfo(scrFileName).isRelative())
        scrPath = m_jsEngine->m_PWD + scrFileName;
    else
        scrPath = scrFileName;

    QString error;
    bool ok = false;

    QFile scrFile(m_jsEngine->m_PWD + scrFileName);

    if (!scrFile.open(QIODevice::ReadOnly))
        error = QString("%1 can't open").arg(scrFileName);
    else {
        QString scrContent = scrFile.readAll();
        scrFile.close();

        QJSValue result = qjsEngine(this)->evaluate(scrContent, scrFileName);
        if (result.isError())
            error = QString("%0,%1: %2")
                        .arg(scrFileName)
                        .arg(result.property("lineNumber").toInt())
                        .arg(result.toString());
        else
            ok = true;
    }

    if (!ok)
        qCritical() << "Include error: " << error;

    return ok;
}

bool JsJSObj::loadModule(const QString& module) const
{
    return m_jsEngine->loadModule(module);
}
