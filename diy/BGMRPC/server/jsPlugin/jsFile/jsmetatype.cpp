#include "jsmetatype.h"
#include "jsfileproto.h"

void registerMetaType (QScriptEngine* jsEngine) {
    qScriptRegisterMetaType < filePtr > (jsEngine, jsCustomDataToScrObj < jsFileProto >,
                                           scrObjToCustomData < jsFileProto >);
}
