#include <QFile>
#include <bgmrpc.h>
#include <QJsonArray>

#include "jsplugin.h"
#include "jsglobalobject.h"

using namespace BGMircroRPCServer;

//jsObj::jsObj(BGMRPC* rpc)
//    :BGMRObject < jsAdaptor > (rpc), JSrpc (rpc)
//{
//    initial ();
//}

jsObj::jsObj() : AutoLoad (false), GlobalMutexLock (true)
{
    initial ();
}

jsObj::~jsObj()
{
    delete JsProcClass;
    delete JsJsObjClass;
    delete JsRPCObjectClass;
}

QString jsObj::objectType() const
{
    return objType ();
}

bool jsObj::procIdentify (BGMRProcedure* p, const QString& method, const QJsonArray& as)
{
    mutex.lock ();
    bool identify = false;
    if (method == "loadJsScript" || method == "lock") {
        if (AutoLoad)
            identify = false;
        else if (Password.isEmpty () || Password == as[0].toString ())
            identify = true;
        else
            identify = false;
    } else {
        QScriptValue jsIdentify = JsEngine.globalObject ().property ("jsIdentify");
        if (jsIdentify.isUndefined () || !jsIdentify.isValid ())
            identify = true;
        else {
            QScriptValueList args;
            args << JsEngine.toScriptValue < BGMRProcedure* > (p)
                 << JsEngine.toScriptValue < QString > (method)
                 << JsEngine.toScriptValue < QJsonArray > (as);

            identify = jsIdentify.call (QScriptValue (), args).toBool ();
        }
    }
    mutex.unlock ();

    return identify;
}

QJsonArray jsObj::loadJsScript(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QString error;
    bool ok = false;

    if (args [0].toDouble () == 0) {
        QString jsPluginDir = BGMRPC::Settings->value ("pluginDir", "~/.BGMR/plugins/").toString ();
        jsPluginDir += "/js/";
        QString jsFileName = jsPluginDir + args[1].toString ();

        ok = loadJsScriptFile (jsFileName, error);
    } else
        ok = loadJsScriptContent (args [1].toString (), error,
                args [2].toString ("noName"));

    ret.append (ok);
    if (!error.isEmpty ())
        ret.append (error);

    return ret;
}

bool jsObj::loadJsScriptFile(const QString& jsFileName, QString& error)
{
    bool ok = false;

    QFile jsFile (jsFileName);
    if (!jsFile.open (QIODevice::ReadOnly))
        error = QString ("%1 can't open").arg (jsFileName);
    else {
        QString jsContent = jsFile.readAll ();
        jsFile.close ();

        ok = loadJsScriptContent (jsContent, error, jsFileName);
    }

    return ok;
}

QJsonArray jsObj::js(BGMRProcedure* p, const QJsonArray& args)
{
    mutex.lock (); // 1 lock
    QJsonArray ret;

    QScriptValue global = JsEngine.globalObject ();
    QScriptValue jsFun = global.property (args[0].toString ());
    mutex.unlock (); // 1 unlock

    if (jsFun.isFunction ()) {
        mutex.lock (); // 2 lock
        QScriptValueList scrArgs;
        scrArgs << JsEngine.toScriptValue (p);
        for (int i = 1; i < args.count (); i++)
            scrArgs << jsonValueToScrObj (&JsEngine, args [i]);
        mutex.unlock (); // 2 unlock

        if (GlobalMutexLock)
            mutex.lock (); // call lock
        QScriptValue scrRet = jsFun.call (QScriptValue (),
                                          scrArgs);
        qDebug () << "\t" + args [0].toString ();
        if (GlobalMutexLock)
            mutex.unlock (); // call unlock

        mutex.lock (); // 3 lock
        QJsonValue jsonRet;
        scrObjToJsonValue (scrRet, jsonRet);
        if (jsonRet.isArray ())
            ret = jsonRet.toArray ();
        else
            ret.append (jsonRet);
        mutex.unlock (); // 3 unlock
    }

//    mutex.unlock ();

    return ret;
}

QJsonArray jsObj::lock(BGMRProcedure*, const QJsonArray& args)
{
    Password = args [0].toString ();

    QJsonArray ret;
    ret.append (true);

    return ret;
}

void jsObj::setGlobalMutexLock(bool l)
{
    GlobalMutexLock = l;
}

bool jsObj::globalMutexLock() const
{
    return GlobalMutexLock;
}

void jsObj::loadModule(const QString& module)
{
    QString moduleDir = BGMRPC::Settings->value ("rootDir", "~/.BGMR/").toString () + "/modules/JS/";

    QLibrary theModuleLib;
    theModuleLib.setFileName (moduleDir + module);

    if (!LoadedModules.contains (module))
        LoadedModules.append (module);
    if (!theModuleLib.isLoaded ())
        theModuleLib.load ();

    void (*initialModule)(QScriptEngine*, BGMRObjectInterface*) = (void (*)(QScriptEngine*, BGMRObjectInterface*))theModuleLib.resolve ("initialModule");
    initialModule (&JsEngine, this);
}

void jsObj::setAutoLoad()
{
    AutoLoad = true;
}

relatedProcs* jsObj::relProcs()
{
    return &RelProcs;
}

void jsObj::setRPC(BGMRPC* rpc)
{
    JSRPC.setRPC (rpc);
    JsEngine.globalObject ().setProperty ("RPC", JsEngine.newQObject (&JSRPC));
    DefaultGlobalProperty.setProperty ("RPC", JsEngine.newQObject (&JSRPC));
}

void jsObj::initial()
{
    QScriptValue globalObject = JsEngine.globalObject ();

    JsJsObjClass = new jsJsObjClass (&JsEngine);
    globalObject.setProperty ("JS", JsJsObjClass->newInstance (this));

    JsProcClass = new jsProcClass (&JsEngine);
    globalObject.setProperty (JsProcClass->name (), JsProcClass->construct ());

    JsRPCObjectClass = new jsRPCObjectClass (&JsEngine);
    globalObject.setProperty (JsRPCObjectClass->name (), JsRPCObjectClass->construct ());

//    JsSqlClass = new jsSqlQueryClass (&JsEngine);
//    globalObject.setProperty (JsSqlClass->name (), JsSqlClass->construct ());

//    globalObject.setProperty ("DB", JsEngine.newQObject (&JsDB));

    registerMetaType (&JsEngine);

    DefaultGlobalProperty = JsEngine.newObject ();
    QScriptValueIterator it (globalObject);
    while (it.hasNext ()) {
        it.next ();
        DefaultGlobalProperty.setProperty (it.name (), it.value ());
    }
}

bool jsObj::loadJsScriptContent(const QString& jsContent, QString& error,
                                const QString& jsFileName)
{
    QString moduleDir = BGMRPC::Settings->value ("rootDir", "~/.BGMR/").toString () + "/modules/JS/";

    QLibrary theModuleLib ;
    foreach (QString theModule, LoadedModules) {
        theModuleLib.setFileName (moduleDir + theModule);
        if (theModuleLib.isLoaded ()) {
            void (*unload)() = (void (*)())theModuleLib.resolve ("unload");
            unload ();
            theModuleLib.unload ();
        }
    }
    LoadedModules.clear ();

    QScriptValue globalObject = JsEngine.globalObject ();
    QScriptValueIterator it (globalObject);
    while (it.hasNext ()) {
        it.next ();
        QString proName = it.name ();
        if (!DefaultGlobalProperty.property (proName).isValid ())
            globalObject.setProperty (proName, QScriptValue ());
    }

    bool ok = false;
    QScriptValue result = JsEngine.evaluate (jsContent, jsFileName);
    if (result.isError ()) {
        error = QString ("%0:%1: %2").arg (jsFileName)
                                         .arg (result.property ("lineNumber").toInt32 ())
                                         .arg (result.toString ());
    } else {
        JsEngine.globalObject ().setProperty ("jsDebug",
                                              JsEngine.newFunction (jsDebug));

        QScriptValue constructFun = globalObject.property ("construct");
        if (constructFun.isFunction ())
            constructFun.call (QScriptValue ());

        ok = true;
    }

    return ok;
}

QScriptValue jsObj::jsDebug(QScriptContext* context, QScriptEngine*)
{
    int argCount = context->argumentCount ();
    if (argCount > 0) {
        for (int i = 0; i < argCount; i++) {
            QScriptValue argV = context->argument (i);
            if (argV.isNumber ())
                qDebug () << argV.toNumber ();
            else if (argV.isBool ())
                qDebug () << argV.toBool ();
            else if (argV.isVariant ())
                qDebug () << argV.toVariant ();
            else if (argV.isQObject ())
                qDebug () << "[QObject]";
            else
                qDebug () << argV.toString ();
        }
    }

    return QScriptValue ();
}

//================

void jsAdaptor::registerMethods()
{
    Methods ["loadJsScript"] = &jsObj::loadJsScript;
    Methods ["js"] = &jsObj::js;
    Methods ["lock"] = &jsObj::lock;
}

//================

BGMRObjectInterface* objCreator ()
{
    return new jsObj;
}

QString objType ()
{
    return "JSEngine";
}


void loadAutoLoadScripts (BGMRObjectStorage* storage, BGMRPC* rpc,
                  const QString& path, bool withRPC = false)
{
    QDir autoloadJsDir (path);
    QFileInfoList jsFiles = autoloadJsDir.entryInfoList (QStringList () << "*.js");

    foreach (QFileInfo jsFile, jsFiles) {
        QString error;

        QString jsObjName = jsFile.baseName ();
        if (!storage->installObject (jsObjName, objType ()))
            qDebug () << "Can't create " + jsObjName + " object";

        jsObj* theJsObj = dynamic_cast < jsObj* > (storage->object (jsObjName));
        if (theJsObj) {
            theJsObj->setAutoLoad ();

            if (withRPC)
                theJsObj->setRPC (rpc);
            if (!theJsObj->loadJsScriptFile (jsFile.filePath (), error))
                qDebug () << "Load js file error: " << error;
        } else
            qDebug () << "Not a jsObj";
    }
}

bool initial (BGMRObjectStorage* storage, BGMRPC* rpc)
{
    QString pluginDir = BGMRPC::Settings->value ("pluginDir", "~/.BGMR/plugins/").toString ();
    loadAutoLoadScripts (storage, rpc, pluginDir + "/js/autoload_admin", true);
    loadAutoLoadScripts (storage, rpc, pluginDir + "/js/autoload");

//    foreach (QFileInfo jsFile, jsFiles) {
//        QString error;

//        QString jsObjName = jsFile.baseName ();
//        if (!storage->installObject (jsObjName, objType ()))
//            qDebug () << "Can't create " + jsObjName + " object";

//        jsObj* theJsObj = dynamic_cast < jsObj* > (storage->object (jsObjName));
//        if (theJsObj) {
//            theJsObj->setRPC (rpc);
//            if (!theJsObj->loadJsScriptFile (jsFile.filePath (), error))
//                qDebug () << "Load js file error: " << error;
//        } else
//            qDebug () << "Not a jsObj";
//    }

    return true;
}
