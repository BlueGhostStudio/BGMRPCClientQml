#ifndef THREADOBJECTINTERFACE_H
#define THREADOBJECTINTERFACE_H

#include "ThreadObjectInterface_global.h"
#include <QMutex>
#include <QThread>
#include <objectinterface.h>

namespace NS_BGMRPCObjectInterface
{

extern QMutex mutex;

class ThreadObjectInterface;

class CallThread : public QThread
{
    Q_OBJECT

public:
    CallThread(ThreadObjectInterface* object, const QString& mid,
               QPointer<Caller> caller, const QString& method,
               const QVariantList& args, QObject* parent = nullptr);

    void run() override;

protected:
    ThreadObjectInterface* m_object;
    QString m_mID;
    QPointer<Caller> m_caller;
    QString m_method;
    QVariantList m_args;
};

class THREADOBJECTINTERFACE_EXPORT ThreadObjectInterface
    : public ObjectInterface
{
    Q_OBJECT
public:
    ThreadObjectInterface(QObject* parent = nullptr);

protected:
    void callMethod(const QString& mID, QPointer<Caller> caller,
                    const QString& methodName,
                    const QVariantList& args) override;

    friend CallThread;
};

} // namespace NS_BGMRPCObjectInterface
#endif // THREADOBJECTINTERFACE_H
