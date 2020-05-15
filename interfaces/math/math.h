#ifndef MATH_H
#define MATH_H

#include <QObject>
#include <objectinterface.h>

namespace NS_BGMRPCObjectInterface
{

class Math : public ObjectInterface
{
    Q_OBJECT

public:
    Math(QObject* parent = nullptr);

    QVariant plus(QPointer<Caller> cli, const QVariantList& args);
    QVariant join(QPointer<Caller> cli, const QVariantList& args);
    QVariant foreachRelatedCaller(QPointer<Caller>, const QVariantList&);
    QVariant broadcastSignal(QPointer<Caller>, const QVariantList&);
    QVariant testThread(QPointer<Caller>, const QVariantList&);

protected:
    bool verification(QPointer<Caller> caller, const QString& method,
                      const QVariantList& args) override;
    void registerMethods() override;
};
} // namespace NS_BGMRPCObjectInterface

extern "C" {
NS_BGMRPCObjectInterface::ObjectInterface* create(int, char**);
}
#endif // MATH_H
