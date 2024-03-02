#ifndef MISSINGOBJECTHANDLER_H
#define MISSINGOBJECTHANDLER_H

#include <objectinterface.h>

#include "MissingObjectHandler_global.h"

using namespace NS_BGMRPCObjectInterface;

class MISSINGOBJECTHANDLER_EXPORT MissingObjectHandler
    : public ObjectInterface {
    Q_OBJECT

public:
    MissingObjectHandler(QObject* parent = nullptr);

    QVariant request(QPointer<Caller> caller, const QString& object,
                     const QString& method, const QVariantList& args);

protected:
    void registerMethods() override;
};

extern "C" {
NS_BGMRPCObjectInterface::ObjectInterface* create(int, char**);
}

#endif  // MISSINGOBJECTHANDLER_H
