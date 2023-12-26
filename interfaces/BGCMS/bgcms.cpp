#include "bgcms.h"

using namespace NS_BGMRPCObjectInterface;

BGCMS::BGCMS(QObject* parent) : ObjectInterface(parent) {}

QVariant
BGCMS::join(QPointer<Caller> caller, const QVariantList&) {
    addRelatedCaller(caller);

    return QVariant();
}

QVariant
BGCMS::node(QPointer<Caller> caller, const QVariantList& args) {
    if (args.length() == 1) {
        return QVariantMap({ { "ok", true }, { "id", QVariant() } });
    } else
        return QVariantMap({ { "ok", true }, { "id", 1 } });
}

QVariant
BGCMS::getCallerToken(QPointer<Caller> caller) {
    return call(caller, "account", "getToken", QVariantList()).toList()[0];
}

void
BGCMS::registerMethods() {
    REG_METHOD("join", &BGCMS::join);
    REG_METHOD("node", &BGCMS::node);
}
