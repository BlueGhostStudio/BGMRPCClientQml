#include "jsmetatype.h"
#include "jsdbmproto.h"

void registerMetaType (QScriptEngine* jsEngine)
{
    qScriptRegisterMetaType < BGMRPCDBModule* > (
                jsEngine,
                jsCustomDataToScrObj < jsDBModuleProto >,
                scrObjToCustomData < jsDBModuleProto);
}
