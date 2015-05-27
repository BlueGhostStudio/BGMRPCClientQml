#include "jsmetatype.h"
#include "jshttpproto.h"

void registerMetaType (QScriptEngine *jsEngine) {
    qScriptRegisterMetaType < jsHttpPtr >
            (jsEngine,
             jsCustomDataToScrObj < jsHttpProto >,
             scrObjToCustomData < jsHttpProto >);
}

