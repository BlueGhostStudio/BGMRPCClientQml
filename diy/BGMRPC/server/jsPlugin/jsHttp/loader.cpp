#include "loader.h"
#include "jshttpproto.h"
#include "jsmetatype.h"

//jsHttp* theJsHttp = NULL;

jsHttpClass* theJsHttpClass = NULL;

static QScriptValue httpInstance (QScriptContext*, QScriptEngine* engine)
{
    httpWork* newHttp = new httpWork;
    newHttp->moveToThread (engine->thread ());
    QScriptValue httpObj = engine->toScriptValue < httpWork* > (newHttp);
    QScriptValue initFun = httpObj.property ("initial");
    if (initFun.isFunction ()) {
        initFun.call (httpObj);
    }
    return httpObj;
//    jsHttp* newHttp = new jsHttp;
//    newHttp->moveToThread (engine->thread ());
//    //QScriptValue v = engine->newQObject (newHttp);
//    return engine->toScriptValue < jsHttp* > (newHttp);
}
static QScriptValue deleteHttpInstance (QScriptContext* context,
                                        QScriptEngine* engine)
{
    httpWork* theInstance
            = engine->fromScriptValue < httpWork* > (context->argument (0));
    if (theInstance) {
        theInstance->deleteLater ();
    }

    return QScriptValue ();
//    jsHttp* theInstance
//            = engine->fromScriptValue < jsHttp* > (context->argument (0));
//    if (theInstance) {
//        qDebug () << "delete Instance";
//        theInstance->deleteLater ();
//    }

//    return QScriptValue ();
}

void initialModule (QScriptEngine* jsEngine, BGMRObjectInterface*)
{
    qDebug () << "loading jsHttp module";
    registerMetaType (jsEngine);
    theJsHttpClass = new jsHttpClass (jsEngine);
    jsEngine->globalObject ().setProperty (theJsHttpClass->name (),
                                           theJsHttpClass->construct ());
    jsEngine->globalObject ().setProperty (
                "httpInstance",
                jsEngine->newFunction (httpInstance));
    jsEngine->globalObject ().setProperty (
                "deleteHttpInstance",
                jsEngine->newFunction (deleteHttpInstance));
}

void unload ()
{
    qDebug () << "unload jsHttp Module";
    delete theJsHttpClass;
}
