#include "jsengine.h"
#include "jsobjs.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <functional>
#include <getopt.h>

using namespace NS_BGMRPCObjectInterface;

JsEngine::JsEngine(QObject* parent)
    : ObjectInterface(parent), m_jsEngine(nullptr)
{
}

QVariant JsEngine::callJs(const QString& /*name*/, QPointer<Caller> /*caller*/,
                          const QVariantList& /*args*/)
{
    return QVariant();
}

bool JsEngine::loadJsFile(const QString& jsFileName)
{
    QFile jsFile(jsFileName);
    if (jsFile.open(QIODevice::ReadOnly)) {
        delete m_jsEngine;
        m_jsEngine = new QJSEngine();
        m_jsEngine->installExtensions(QJSEngine::ConsoleExtension);
        QJSValue result = m_jsEngine->evaluate(jsFile.readAll(), jsFileName);

        if (result.isError()) {
            qDebug() << QString("%1,%2: %3")
                            .arg(result.property("fileName").toString())
                            .arg(result.property("lineNumber").toInt())
                            .arg(result.toString());

            return false;
        } else {
            registerMethods();

            m_PWD = QFileInfo(jsFileName).path() + '/';
            QJSValue jsObj = m_jsEngine->newQObject(new JsJSObj(this));
            m_jsEngine->globalObject().setProperty("JS", jsObj);

            m_jsVerificationFun =
                m_jsEngine->globalObject().property("verification");
            if (m_jsVerificationFun.isUndefined())
                m_jsVerificationFun =
                    m_jsEngine->globalObject().property("clientIdentify");

            QJSValue jsConstructor =
                m_jsEngine->globalObject().property("constructor");
            if (jsConstructor.isCallable())
                jsConstructor.call();

            return true;
        }
    } else
        return true;
}

bool JsEngine::loadModule(const QString& module)
{
    QLibrary theModuleLib;
    QString modulePath;
    if (QFileInfo(module).isReadable())
        modulePath = m_PWD + module;
    else
        modulePath = module;

    if (!m_loadedModules.contains(module)) {
        m_loadedModules.append(module);
        theModuleLib.setFileName(modulePath);
        if (theModuleLib.load()) {
            void (*initialModule)(QJSEngine*) =
                (void (*)(QJSEngine*))theModuleLib.resolve("initialModule");
            initialModule(m_jsEngine);
            return true;
        } else {
            qWarning() << "Can't load module: " << modulePath;
            return false;
        }
    } else {
        qWarning() << "The moudule has loaded: " << modulePath;
        return false;
    }
}

QJSValue JsEngine::variant2JsValue(const QVariant& var)
{
    switch (var.type()) {
    case QVariant::Bool:
        return QJSValue(var.toBool());
    case QVariant::Int:
    case QVariant::LongLong:
        return QJSValue(var.toInt());
    case QVariant::UInt:
    case QVariant::ULongLong:
        return QJSValue(var.toUInt());
    case QVariant::Double:
        return QJSValue(var.toDouble());
    case QVariant::String:
        return QJSValue(var.toString());
    case QVariant::List: {
        QVariantList varArray = var.toList();
        QJSValue jsArray = m_jsEngine->newArray(varArray.length());
        for (int i = 0; i < varArray.length(); i++)
            jsArray.setProperty(i, variant2JsValue(varArray.at(i)));

        return jsArray;
    }
    case QVariant::Map: {
        QVariantMap varMap = var.toMap();
        QJSValue jsObj = m_jsEngine->newObject();

        QVariantMap::const_iterator it;
        for (it = varMap.constBegin(); it != varMap.constEnd(); it++)
            jsObj.setProperty(it.key(), variant2JsValue(it.value()));

        return jsObj;
    }
    default:
        return QJSValue();
    }
}

void JsEngine::registerMethods()
{
    QJSValue methods = m_jsEngine->globalObject().property("methods");
    if (!methods.isNull()) {
        qInfo().noquote() << "Register Methods ...";
        foreach (QVariant method, methods.toVariant().toList()) {
            QString mn = method.toString();
            QJSValue m = m_jsEngine->globalObject().property(mn);
            if (m.isCallable()) {
                qInfo().noquote() << "..." << mn;
                //                m_jsMethods[mn] = m;
                registerMethod(mn);
            }
            //            qInfo().noquote() << "..." << mn;
            //            registerMethod(mn);
            //            QJSValue m = m_jsEngine->globalObject().property(mn);
            //            m_jsMethods[mn] = m;
        }
    }
}

void JsEngine::registerMethod(const QString& methodName)
{
    m_methods[methodName] = std::bind(
        [&](const QString& name, ObjectInterface* /*oif*/,
            QPointer<Caller> caller, const QVariantList &
            /*args*/) -> QVariant {
            if (caller.isNull())
                return QVariant();
            QMutexLocker locker(&m_mutex);

            qDebug() << m_jsEngine->globalObject().property(name).isCallable();

            return QVariant();
            //            return qobject_cast<JsEngine*>(oif)->callJs(name,
            //            caller, args);
        },
        methodName, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);
}

bool JsEngine::verification(QPointer<Caller> caller, const QString& method,
                            const QVariantList& args)
{
    /*QMutexLocker locker(&m_mutex);
    if (m_jsVerificationFun.isCallable()) {
        JsCaller* theJsCaller = new JsCaller(caller);
        theJsCaller->deleteLater();
        return m_jsVerificationFun
            .call(QJSValueList() << m_jsEngine->newQObject(theJsCaller)
                                 << method << m_jsEngine->toScriptValue(args))
            .toBool();
    } else*/
    return true;
}

ObjectInterface* create(int argc, char** argv)
{
    int opt = 0;
    optind = 0;
    QString jsFile;

    while ((opt = getopt(argc, argv, "j:")) != -1) {
        switch (opt) {
        case 'j':
            jsFile = optarg;
            break;
        }
    }

    JsEngine* obj = new JsEngine;
    obj->loadJsFile(jsFile);

    return obj;
}
