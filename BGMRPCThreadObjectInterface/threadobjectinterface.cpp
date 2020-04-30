#include "threadobjectinterface.h"

using namespace NS_BGMRPCObjectInterface;

QMutex mutex;

CallThread::CallThread(ThreadObjectInterface* object, const QString& mid,
                       QPointer<Caller> caller, const QString& method,
                       const QVariantList& args, QObject* parent)
    : QThread(parent), m_object(object), m_mID(mid), m_caller(caller),
      m_method(method), m_args(args)

{
}

void CallThread::run()
{
    m_object->ObjectInterface::callMethod(m_mID, m_caller, m_method, m_args);
}

ThreadObjectInterface::ThreadObjectInterface(QObject* parent)
    : ObjectInterface(parent)
{
}

void ThreadObjectInterface::callMethod(const QString& mID,
                                       QPointer<Caller> caller,
                                       const QString& methodName,
                                       const QVariantList& args)
{
    CallThread* callThread =
        new CallThread(this, mID, caller, methodName, args);

    callThread->start();
}
