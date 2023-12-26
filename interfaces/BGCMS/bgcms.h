#ifndef BGCMS_H
#define BGCMS_H

#include <objectinterface.h>
#include <QObject>

#include "BGCMS_global.h"

namespace NS_BGMRPCObjectInterface {

class BGCMSAPP_EXPORT BGCMS : public ObjectInterface
{
    Q_OBJECT
public:
    explicit BGCMS(QObject* parent = nullptr);

    QVariant join(QPointer<Caller> caller, const QVariantList&);

    QVariant node(QPointer<Caller> caller, const QVariantList& args);

private:
    QVariant getCallerToken(QPointer<Caller> caller);

protected:
    void registerMethods() override;
};


}
#endif // BGCMS_H
