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

jsObj::jsObj() : JsDB (this)
{
    initial ();
}

QString jsObj::objectType() const
{
    return objType ();
}

bool jsObj::procIdentify(BGMRProcedure* p, const QJsonObject& call)
{
    QScriptValue jsIdentify = JsEngine.globalObject ().property ("jsIdentify");
    if (jsIdentify.isUndefined () || !jsIdentify.isValid ())
        return true;
    else {
        QScriptValueList args;
        args << JsEngine.toScriptValue < BGMRProcedure* > (p)
             << JsEngine.toScriptValue < QJsonObject > (call);

        return jsIdentify.call (QScriptValue (), args).toBool ();
    }
}

QJsonArray jsObj::loadJsScript(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QString error;
    bool ok = false;

    qDebug () << "in loadJsScript";

    if (args [0].toDouble () == 0) {
        QString jsPluginDir = BGMRPC::Settings->value ("pluginDir", "~/.BGMR/plugins/").toString ();
        jsPluginDir += "/js/";
        QString jsFileName = jsPluginDir + args[1].toString ();
        qDebug () << jsFileName;

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
    QJsonArray ret;

    QScriptValue global = JsEngine.globalObject ();
    QScriptValue jsFun = global.property (args[0].toString ());

    if (jsFun.isFunction ()) {
        QScriptValueList scrArgs;
        scrArgs << JsEngine.toScriptValue (p);
        for (int i = 1; i < args.count (); i++)
            scrArgs << jsonValueToScrObj (&JsEngine, args [i]);

        QScriptValue scrRet = jsFun.call (QScriptValue (),
                                          scrArgs);
        QJsonValue jsonRet;
        scrObjToJsonValue (scrRet, jsonRet);
        if (jsonRet.isArray ())
            ret = jsonRet.toArray ();
        else
            ret.append (jsonRet);
    }

    return ret;
}

relatedProcs* jsObj::relProcs()
{
    return &RelProcs;
}

void jsObj::setRPC(BGMRPC* rpc)
{
    JSRPC.setRPC (rpc);
    JsEngine.globalObject ().setProperty ("RPC", JsEngine.newQObject (&JSRPC));
    v.setProperty ("RPC", JsEngine.newQObject (&JSRPC));
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

    JsSqlClass = new jsSqlQueryClass (&JsEngine);
    globalObject.setProperty (JsSqlClass->name (), JsSqlClass->construct ());

    globalObject.setProperty ("DB", JsEngine.newQObject (&JsDB));

    registerMetaType (&JsEngine);

    v = JsEngine.newObject ();
    QScriptValueIterator it (globalObject);
    while (it.hasNext ()) {
        it.next ();
        v.setProperty (it.name (), it.value ());
    }
}

bool jsObj::loadJsScriptContent(const QString& jsContent, QString& error,
                                const QString& jsFileName)
{
    QScriptValue globalObject = JsEngine.globalObject ();
    QScriptValueIterator it (globalObject);
    while (it.hasNext ()) {
        it.next ();
        QString proName = it.name ();
        if (!v.property (proName).isValid ())
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
