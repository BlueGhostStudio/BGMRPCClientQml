#include "loader.h"
#include "jsmetatype.h"
#include "jsfileproto.h"

jsFileClass* theSqlQueryClass = NULL;
void initialModule (QScriptEngine* jsEngine, BGMRObjectInterface*) {
    registerMetaType (jsEngine);

    QScriptValue globalObject = jsEngine->globalObject ();
    QScriptValue fileFlag = jsEngine->newObject ();
    fileFlag.setProperty ("READONLY", QIODevice::ReadOnly);
    fileFlag.setProperty ("WRITEONLY", QIODevice::WriteOnly);
    fileFlag.setProperty ("READWRITE", QIODevice::ReadWrite);
    fileFlag.setProperty ("APPEND", QIODevice::Append);
    fileFlag.setProperty ("TRUNCATE", QIODevice::Truncate);
    fileFlag.setProperty ("TEXT", QIODevice::Text);
    fileFlag.setProperty ("UNBUFFERED", QIODevice::Unbuffered);
    globalObject.setProperty ("fileFlag", fileFlag);

    theSqlQueryClass = new jsFileClass (jsEngine);
    globalObject.setProperty (theSqlQueryClass->name (), theSqlQueryClass->construct ());
}


void unload()
{
    delete theSqlQueryClass;
}
