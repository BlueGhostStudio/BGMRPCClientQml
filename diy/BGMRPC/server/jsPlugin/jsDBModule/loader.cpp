#include "loader.h"
#include "jsmetatype.h"
#include "jsdbmproto.h"

jsDBModuleClass* theDBModuleClass = NULL;

void initialModule (QScriptEngine* jsEngine, BGMRObjectInterface* obj)
{
    registerMetaType (jsEngine);

    theDBModuleClass = new jsDBModuleClass (jsEngine);
    jsEngine->globalObject ().setProperty (theDBModuleClass->name (),
                                           theDBModuleClass->construct ());
}

void unload ()
{
    delete theDBModuleClass;
}
