#include "jsmetatype.h"
#include "jsdbproto.h"

void registerMetaType (QScriptEngine *jsEngine)
{
    qScriptRegisterMetaType < QSqlQuery > (jsEngine, jsCustomDataToScrObj < jsSqlQueryProto >,
                                           scrObjToCustomData < jsSqlQueryProto >);
}
