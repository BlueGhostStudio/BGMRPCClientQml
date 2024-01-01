#include "jsengine.h"

#include <getopt.h>

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QLibrary>
#include <QSettings>
#include <functional>

#include "jsobjs.h"

using namespace NS_BGMRPCObjectInterface;

JsEngine::JsEngine(QObject* parent)
    : ObjectInterface(parent), m_jsEngine(nullptr) {}

QVariant
JsEngine::callJs(const QString& name, QPointer<Caller> caller,
                 const QVariantList& args) {
    QMutexLocker locker(&m_mutex);
    QJSValue method = m_jsEngine->globalObject().property(name);
    if (method.isCallable()) {
        QJSValueList jsArgs;
        jsArgs << m_jsEngine->newQObject(new JsCaller(caller));
        foreach (QVariant a, args) jsArgs << m_jsEngine->toScriptValue(a);

        return method.call(jsArgs).toVariant();
    } else
        return QVariant();
    //    return QVariant();
}

bool
JsEngine::loadJsFile(const QString& jsFileName) {
    qInfo().noquote() << "load Js Script" << jsFileName;
    QFile jsFile(jsFileName);
    if (jsFile.open(QIODevice::ReadOnly)) {
        delete m_jsEngine;
        m_jsEngine = new QJSEngine();
        m_jsEngine->installExtensions(QJSEngine::ConsoleExtension);
        QJSValue result = m_jsEngine->evaluate(jsFile.readAll(), jsFileName);

        if (result.isError()) {
            qCritical().noquote()
                << QString("%1,%2: %3")
                       .arg(result.property("fileName").toString())
                       .arg(result.property("lineNumber").toInt())
                       .arg(result.toString());

            return false;
        } else {
            registerMethods();

            m_PWD = QFileInfo(jsFileName).absolutePath() + '/';
            QJSValue jsObj = m_jsEngine->newQObject(new JsJSObj(this));
            m_jsEngine->globalObject().setProperty("JS", jsObj);

            m_jsVerificationFun =
                m_jsEngine->globalObject().property("verification");

            QJSValue jsConstructor =
                m_jsEngine->globalObject().property("constructor");
            if (jsConstructor.isCallable()) jsConstructor.call();

            return true;
        }
    } else
        return false;
}

QString
JsEngine::modulesPath() const {
    return m_jsModulesPath;
}

QString
JsEngine::PWD() const {
    // return QDir::currentPath();
    return m_PWD;
}

void
JsEngine::loadModule(const QString& module) {
    QLibrary theModuleLib;
    /*QString modulePath;
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
    }*/
    auto __loadModule = [&](const QString& lib) -> bool {
        theModuleLib.unload();
        theModuleLib.setFileName(lib);
        bool ok = theModuleLib.isLoaded() || theModuleLib.load();
        if (!ok) {
            qWarning().noquote() << "Can't load module lib: " << lib << ". "
                                 << theModuleLib.errorString();
        } else
            qInfo().noquote() << lib << "loaded";

        return ok;
    };

    if (!m_loadedModules.contains(module)) m_loadedModules.append(module);

    bool ok = __loadModule(m_PWD + module) ||
              __loadModule(m_jsModulesPath + '/' + module);
    /*if (!__loadModule(m_appPath + '/' + module))
        ok = __loadModule(m_jsModulesPath + '/' + module);
    else
        ok = true;*/

    if (ok) {
        void (*initialModule)(QJSEngine*) =
            (void (*)(QJSEngine*))theModuleLib.resolve("initialModule");
        initialModule(m_jsEngine);
    }
    /*QString modulePath = m_jsModulesPath + '/' + module;
    theModuleLib.setFileName(modulePath);

    if (!m_loadedModules.contains(module)) m_loadedModules.append(module);

    if (!theModuleLib.isLoaded() && !theModuleLib.load()) {
        qWarning() << "Can't load module lib: "
                   << m_jsModulesPath + '/' + module;
        return;
    }*/
}

QJSValue
JsEngine::variant2JsValue(const QVariant& var) {
    switch (var.typeId()) {
    case QMetaType::Bool:  // QVariant::Bool:
        return QJSValue(var.toBool());
    case QMetaType::Int:
    case QMetaType::LongLong:
        return QJSValue(var.toInt());
    case QMetaType::UInt:
    case QMetaType::ULongLong:
        return QJSValue(var.toUInt());
    case QMetaType::Double:
        return QJSValue(var.toDouble());
    case QMetaType::QString:
        return QJSValue(var.toString());
    case QMetaType::QVariantList: {
        QVariantList varArray = var.toList();
        QJSValue jsArray = m_jsEngine->newArray(varArray.length());
        for (int i = 0; i < varArray.length(); i++)
            jsArray.setProperty(i, variant2JsValue(varArray.at(i)));

        return jsArray;
    }
    case QMetaType::QVariantMap: {
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

bool
JsEngine::initial(int argc, char** argv) {
    QString rootPath = getSettings(*m_objectPlug, NS_BGMRPC::CNF_PATH_ROOT);
    QString installDir = getSettings(*m_objectPlug, "path/installDir");
    QString etcDir = getSettings(*m_objectPlug, NS_BGMRPC::CNF_PATH_ETC);

    QSettings JSMSettings(etcDir + "/JsModules.conf",
                          QSettings::IniFormat);
    m_jsModulesPath =
        JSMSettings.value("path", installDir + "/JsModules.conf").toString();

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

    bool loadJsFileOk = loadJsFile(m_appPath + '/' + jsFile);

    return loadJsFileOk;
}

void
JsEngine::registerMethods() {
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

void
JsEngine::registerMethod(const QString& methodName) {
    m_methods[methodName] =
        std::bind(&JsEngine::callJs, this, methodName, std::placeholders::_1,
                  std::placeholders::_2);
}

bool
JsEngine::verification(QPointer<Caller> caller, const QString& method,
                       const QVariantList& args) {
    QMutexLocker locker(&m_mutex);
    if (m_jsVerificationFun.isCallable()) {
        JsCaller* theJsCaller = new JsCaller(caller);
        theJsCaller->deleteLater();
        return m_jsVerificationFun
            .call(QJSValueList() << m_jsEngine->newQObject(theJsCaller)
                                 << method << m_jsEngine->toScriptValue(args))
            .toBool();
    } else
        return true;
}

ObjectInterface*
create(/*int argc, char** argv*/) {
    /*int opt = 0;
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

    return obj;*/
    return new JsEngine;
}
