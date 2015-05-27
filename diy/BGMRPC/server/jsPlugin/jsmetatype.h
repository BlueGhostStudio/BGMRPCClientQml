#ifndef JSMETATYPE_H
#define JSMETATYPE_H

#include <QJsonArray>
#include <QJsonValue>
#include <QtScript>
//#include "jsprocclass.h"
#include "jsobjectclass.h"
#include "jsmetatypecommon.h"
#include <relatedproc.h>
//#include "jsproc.h"

using namespace BGMircroRPCServer;
typedef QMap < qulonglong, BGMRProcedure* > relProcsMap;
Q_DECLARE_METATYPE (relProcsMap)
typedef QMap < QString, BGMRObjectInterface* > RPCObjList;
Q_DECLARE_METATYPE (RPCObjList)
Q_DECLARE_METATYPE (QJsonValue)

QScriptValue jsonToScr (QScriptEngine* jsEngine,
                        const QJsonValue& jsonValue);
QJsonDocument scrToJson (const QScriptValue& scrValue);

QScriptValue jsonValueToScrObj (QScriptEngine* jsEngine, const QJsonValue& jsonValue);
void scrObjToJsonValue(const QScriptValue& scrValue, QJsonValue& jsonValue);

QScriptValue jsonArrayToScrObj (QScriptEngine* jsEngine, const QJsonArray& jsonArray);
void scrObjToJsonArray(const QScriptValue& scrValue, QJsonArray& jsonArray);

QScriptValue jsonObjToScrObj (QScriptEngine* jsEngine, const QJsonObject& jsonObj);
void scrObjToJsonObj (const QScriptValue& scrValue, QJsonObject& jsonObj);

QScriptValue mapToScrObj (QScriptEngine* jsEngine, const QVariantMap& map);
void scrObjToMap (const QScriptValue& scrObj, QVariantMap& map);

QScriptValue jsRelProcsToScrObj (QScriptEngine* jsEngine,
                                 const relProcsMap& relProcs);
void scrObjToJsRelProcs (const QScriptValue& scrObj, relProcsMap&relProcs);

QScriptValue jsRPCObjListToScrObj (QScriptEngine* jsEngine,
                                   const RPCObjList& objList);
void scrObjToJsRPCObjList (const QScriptValue&scrObj, RPCObjList&objList);

void scrObjToBGMRObj (const QScriptValue& scrObj, BGMRObjectInterface*& data);

void registerMetaType (QScriptEngine* jsEngine);

#endif // MISFUNCTION_H
