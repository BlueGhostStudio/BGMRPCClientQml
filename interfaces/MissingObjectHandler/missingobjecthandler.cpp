#include "missingobjecthandler.h"

using namespace NS_BGMRPCObjectInterface;

MissingObjectHandler::MissingObjectHandler(QObject* parent)
    : ObjectInterface(parent) {}

QVariant
MissingObjectHandler::request(QPointer<Caller> caller, const QString& object,
                              const QString& method, const QVariantList& args) {
    return QVariantMap{ { "error", "no object" },
                        { "object", object },
                        { "method", method } };
}

QVariant
MissingObjectHandler::testAsync(QPointer<Caller> caller,
                                const QVariantMap& callInfo,
                                const QVariant& testData) {
    asyncReturn(caller, callInfo, testData);

    return QVariant();
}

void
MissingObjectHandler::registerMethods() {
    RM("request", { "Request call method no exist in object" },
       &MissingObjectHandler::request, ARG<QString>("object"),
       ARG<QString>("method"), ARG<QVariantList>("args"));
    RM("testAsync", { "test async call", true },
       &MissingObjectHandler::testAsync, ARG<QVariantMap>("callInfo"),
       ARG<QVariant>("testData"));
}

ObjectInterface*
create(int, char**) {
    return new MissingObjectHandler;
}
