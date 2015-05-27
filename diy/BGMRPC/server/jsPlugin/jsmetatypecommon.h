#ifndef JSMETATYPECOMMON_H
#define JSMETATYPECOMMON_H

#include <QJsonArray>
#include <QJsonValue>
#include <QtScript>
#include "jsobjectclass.h"
#include <relatedproc.h>

using namespace BGMircroRPCServer;

template < typename PT >
QScriptValue jsCustomDataToScrObj (QScriptEngine* jsEngine,
                                   const typename protoTypeInfo < PT >::dataType& data)
{
    if (protoTypeInfo < PT >::isNull (data))
        return jsEngine->nullValue ();

    QScriptValue ctor
            = jsEngine->globalObject ().property (protoTypeInfo < PT >::className ());
    jsObjectClass < PT >* cls = qscriptvalue_cast < jsObjectClass < PT >* >(ctor.data ());
    if (!cls)
        return jsEngine->newVariant (QVariant::fromValue (data));
    else
        return cls->newInstance (data);
}

template < typename PT >
void scrObjToCustomData (const QScriptValue& scrObj,
                         typename protoTypeInfo < PT >::dataType& data)
{
    if (scrObj.isUndefined () || scrObj.isNull ())
        data = protoTypeInfo < PT >::nullData ();
    else
        data = qvariant_cast < typename protoTypeInfo < PT >::dataType > (scrObj.data ().toVariant ());
}

#endif // JSMETATYPECOMMON_H
