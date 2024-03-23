#include "missingobjecthandler.h"

#include <flags.h>

using namespace NS_BGMRPCObjectInterface;

MissingObjectHandler::MissingObjectHandler(QObject* parent)
    : ObjectInterface(parent) {}

QVariant
MissingObjectHandler::request(QPointer<Caller> caller,
                              const QVariantMap& callInfo,
                              const QString& object, const QString& method,
                              const QVariantList& args) {
    /*return QVariantMap{ { "error", "no object" },
                        { "object", object },
                        { "method", method } };*/
    asyncReturnError(caller, callInfo, NS_BGMRPC::ERR_NOOBJ,
                     QString("No exist object the name is %1").arg(object));

    return QVariant();
}

void
MissingObjectHandler::registerMethods() {
    RM("request", { "Request call method no exist in object", true },
       &MissingObjectHandler::request, ARG<QVariantMap>("callInfo"),
       ARG<QString>("object"), ARG<QString>("method"),
       ARG<QVariantList>("args"));
}

ObjectInterface*
create(int, char**) {
    return new MissingObjectHandler;
}
