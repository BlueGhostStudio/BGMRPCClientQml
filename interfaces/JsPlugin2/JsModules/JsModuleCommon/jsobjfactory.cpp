#include "jsobjfactory.h"

JsObjFactory::JsObjFactory(QObject* parent, bool autoDestory)
    : QObject(parent), AutoDestory(autoDestory) {}

QJSValue
JsObjFactory::newInstance(const QVariant& args) const {
    QObject* newObj = constructor(args);
    if (AutoDestory) newObj->deleteLater();
    return qjsEngine(this)->newQObject(newObj);
}
