#include "jsengine.h"
#include "jsobjs.h"
#include <bgmrpc.h>
#include <QDebug>

JsEngine::JsEngine():GlobalMutexLock (true)
{
    Engine = NULL;
}

JsEngine::~JsEngine ()
{
}

bool JsEngine::clientIdentify(BGMRClient* cli, const QString& method,
                            const QJsonArray& as, bool lc)
{
    qDebug () << "local call is" << lc;
    if (lc && method != "ljs")
        return false;

    bool identify = false;

    if (!lc) {
        mutex.lock ();
        qDebug () << "vvv identify lock vvv";
    }

    QJSValue jsIdentify = Engine->globalObject ().property ("clientIdentify");
    if (jsIdentify.isCallable ()) {
        JsClient* _jsClientObj = new JsClient (cli);
        QJSValue jsClientObj = Engine->newQObject(_jsClientObj);
        _jsClientObj->deleteLater();
        identify = jsIdentify.call (QJSValueList () << jsClientObj
                                    << method
                                    << Engine->toScriptValue (as)
                                    << lc).toBool ();
    } else
        identify = true;

    if (!lc) {
        qDebug () << "^^^ identify unlock ^^^";
        mutex.unlock ();
    }

    return identify;
}

QString JsEngine::objectType() const
{
    return objType();
}

void JsEngine::setRpc(BGMRPC* rpc)
{
    RPC = rpc;
}

BGMRPC*JsEngine::rpc() const
{
    return RPC;
}

void JsEngine::setObjPath(const QString& path)
{
    ObjPath = path;
}

QString JsEngine::objPath() const
{
    return ObjPath;
}

bool JsEngine::loadJSFile(const QString& jsFileName, QString& error,
                          const QJSValueList& cstrArgs)
{
    QFile jsFile (jsFileName);
    if (jsFile.open(QIODevice::ReadOnly)) {
        delete Engine;
        Engine = new QJSEngine ();
        Engine->installExtensions(QJSEngine::ConsoleExtension);
        QJSValue result = Engine->evaluate(jsFile.readAll(), jsFileName);
        if (result.isError()) {
            error = QString ("%1,%2: %3")
                    .arg (result.property("fileName").toString())
                    .arg (result.property("lineNumber").toInt())
                    .arg (result.toString());
            return false;
        } else {
            QJSValue JSObj = Engine->newQObject(new JsJSObj (this));
            Engine->globalObject().setProperty("JS", JSObj);

            QJSValue _RPC = Engine->newQObject (new JsRPC (RPC));
            Engine->globalObject ().setProperty ("RPC", _RPC);

            QJSValue _constructor
                    = Engine->globalObject ().property ("constructor");
            if (_constructor.isCallable ()) {
                _constructor.call (cstrArgs);
            }

            return true;
        }
    } else {
        error = "Can't open jsFile";
        return false;
    }
}

void JsEngine::loadModule(const QString& module)
{
    QString moduleDir
            = BGMRPC::Settings->value ("rootDir",
                                       "~/.BGMR/").toString () + "/JsModules/";

    QLibrary theModuleLib;
    theModuleLib.setFileName (moduleDir + module);

    if (!LoadedModules.contains (module))
        LoadedModules.append (module);
    if (!theModuleLib.isLoaded ()) {
        if (!theModuleLib.load ()) {
            qDebug () << "Can't load module lib: " << moduleDir + module;
            return;
        }
    }

    void (*initialModule)(QJSEngine*)
            = (void (*)(
                   QJSEngine*)
               )theModuleLib.resolve ("initialModule");
    initialModule (Engine);
}

QJsonArray JsEngine::js(BGMRClient* p, const QJsonArray& args)
{
    return js (p, args, false);
}

QJsonArray JsEngine::ljs(BGMRClient* p, const QJsonArray& args)
{
    return js (p, args, true);
}
/*
QJsonArray JsEngine::js(BGMRClient* p, const QJsonArray& args)
{
    qDebug () << "\t" + args [0].toString ();
    QJsonArray ret;

    if (!Engine)
        return ret;

    mutex.lock();
    qDebug () << "vvv lock 1 vvv";
    QJSValue global = Engine->globalObject();
    QJSValue jsFun = global.property(args[0].toString());
    qDebug () << "^^^ unlock 1 ^^^";
    mutex.unlock ();

    if (jsFun.isCallable ()) {
        mutex.lock ();
        qDebug () << "vvv lock 2 vvv";
        QJSValueList jsArgs;

        JsClient* _jsClientObj = new JsClient (p);
        QJSValue jsClientObj = Engine->newQObject(_jsClientObj);
        _jsClientObj->deleteLater();

        jsArgs << jsClientObj;
        //jsArgs << Engine->toScriptValue (p);
        for (int i = 1; i < args.count(); i++)
            jsArgs << Engine->toScriptValue (args.at(i));
        qDebug () << "^^^ unlock 2 ^^^";
        mutex.unlock();

        if (GlobalMutexLock) {
            mutex.lock();
            qDebug () << "vvv call lock vvv";
        }
        QJSValue jsRet = jsFun.call(jsArgs);
        if (GlobalMutexLock) {
            qDebug () << "^^^ call unlock ^^^";
            mutex.unlock();
        }

        mutex.lock();
        qDebug () << "vvv lock 3 vvv";
        QJsonValue ret_jsv = QJsonValue::fromVariant(jsRet.toVariant());
        if (jsRet.isArray())
            ret = ret_jsv.toArray();
        else
            ret.append(ret_jsv);
        qDebug () << "^^^ unlock 3 ^^^";
        mutex.unlock();
    }

    return ret;
}
*/
void JsEngine::setGlobalMutexLock(bool lock)
{
    GlobalMutexLock = lock;
}

bool JsEngine::globalMutexLock() const
{
    return GlobalMutexLock;
}

relatedClients* JsEngine::relClients ()
{
    return &RelClients;
}

QJsonArray JsEngine::js(BGMRClient* p, const QJsonArray& args, bool lc)
{
    qDebug () << "\t" + args [0].toString ();
    QJsonArray ret;

    if (!Engine)
        return ret;

    if (!lc) {
        mutex.lock();
        qDebug () << "vvv lock 1 vvv";
    }
    QJSValue global = Engine->globalObject();
    QJSValue jsFun = global.property(args[0].toString());
    if (!lc) {
        qDebug () << "^^^ unlock 1 ^^^";
        mutex.unlock ();
    }

    if (jsFun.isCallable ()) {
        if (!lc) {
            mutex.lock ();
            qDebug () << "vvv lock 2 vvv";
        }
        QJSValueList jsArgs;

        JsClient* _jsClientObj = new JsClient (p);
        QJSValue jsClientObj = Engine->newQObject(_jsClientObj);
        _jsClientObj->deleteLater();

        jsArgs << jsClientObj;
        //jsArgs << Engine->toScriptValue (p);
        for (int i = 1; i < args.count(); i++)
            jsArgs << Engine->toScriptValue (args.at(i));
        if (!lc) {
            qDebug () << "^^^ unlock 2 ^^^";
            mutex.unlock();
        }

        if (GlobalMutexLock && !lc) {
            mutex.lock();
            qDebug () << "vvv call lock vvv";
        }
        QJSValue jsRet = jsFun.call(jsArgs);
        if (GlobalMutexLock && !lc) {
            qDebug () << "^^^ call unlock ^^^";
            mutex.unlock();
        }

        if (!lc) {
            mutex.lock();
            qDebug () << "vvv lock 3 vvv";
        }
        QJsonValue ret_jsv = QJsonValue::fromVariant(jsRet.toVariant());
        if (jsRet.isArray())
            ret = ret_jsv.toArray();
        else
            ret.append(ret_jsv);

        if (!lc) {
            qDebug () << "^^^ unlock 3 ^^^";
            mutex.unlock();
        }
    }

    return ret;
}


void JsEngineAdaptor::registerMethods()
{
    Methods ["js"] = &JsEngine::js;
    Methods ["ljs"] = &JsEngine::ljs;
    //Methods ["lock"] = &jsObj::lock;
}


BGMRObjectInterface* objCreator ()
{
    return new JsEngine;
}

QString objType ()
{
    return "JSEngine";
}

bool initial (BGMRObjectStorage* storage, BGMRPC* rpc)
{
    QDir jsObjsDir (BGMRPC::Settings->value ("JSObjs",
                                             "~/.BGMRPC/JSObjs").toString());
    foreach (QFileInfo objDir, jsObjsDir.entryInfoList (QDir::NoDotAndDotDot
                                                        | QDir::Dirs)) {
        QString objName = objDir.baseName();
        BGMRObjectInterface* obj = storage->installObject(objName, objType());
        if (!obj) {
            qDebug () << "Can't create " + objName + " object";
            continue;
        }
        JsEngine* theJsObj
                = dynamic_cast < JsEngine* > (obj);
        if (theJsObj) {
            theJsObj->setRpc (rpc);
            QString error;
            QString objPath = objDir.filePath () + "/";
            theJsObj->setObjPath (objPath);
            if (!theJsObj->loadJSFile (objPath + "main.js", error))
                qDebug () << "Load js error: " << error;
        } else
            qDebug ()  << "Not a jsObj";
    }

    return true;
}
