#include "jsobjfactory.h"

JsObjFactory::JsObjFactory(QObject *parent, bool autoDestory)
    : QObject(parent), AutoDestory (autoDestory)
{}

QJSValue JsObjFactory::newInstance() const
{
    QObject* newObj = constructor();
    if (AutoDestory)
        newObj->deleteLater();
    return qjsEngine(this)->newQObject(newObj);
}

