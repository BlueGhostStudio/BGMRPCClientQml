#include "loader.h"
#include "jsmetatype.h"
#include "jsdbproto.h"

jsSqlQueryClass* theSqlQueryClass = NULL;
jsDB* theJsDB = NULL;

void initialModule (QScriptEngine *jsEngine, BGMRObjectInterface* obj)
{
    registerMetaType (jsEngine);

    qDebug () << "loading jsDB Module";
    QScriptValue globalObject = jsEngine->globalObject ();
    theSqlQueryClass = new jsSqlQueryClass (jsEngine);
    globalObject.setProperty (theSqlQueryClass->name (), theSqlQueryClass->construct ());

    theJsDB = new jsDB (obj);
    globalObject.setProperty ("DB", jsEngine->newQObject (theJsDB));
}

void unload ()
{
    qDebug () << "unload jsDB Module";
    delete theSqlQueryClass;
    delete theJsDB;
}
