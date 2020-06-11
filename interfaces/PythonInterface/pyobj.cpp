#include "pyobj.h"
#include <PythonQt.h>
#include <QObject>

using namespace NS_BGMRPCObjectInterface;

PyCaller::PyCaller(QPointer<Caller> caller, QObject* parent)
    : QObject(parent), m_caller(caller)
{
}

PyCaller::~PyCaller()
{
}

qint64 PyCaller::ID() const
{
    if (m_caller.isNull())
        return 0;
    else
        return m_caller->ID();
}

bool PyCaller::online() const
{
    if (m_caller.isNull())
        return false;
    else
        return !m_caller->exited();
}

void PyCaller::emitSignal(const QString& signal, const QVariant& args) const
{
    if (!m_caller.isNull())
        m_caller->emitSignalReady(signal, args);
}

NS_BGMRPCObjectInterface::PyCaller* PyCaller::clone() const
{
    return new PyCaller(m_caller);
}

void PyCaller::destory()
{
    deleteLater();
}

QPointer<Caller> PyCaller::caller() const
{
    return m_caller;
}

PyObj::PyObj(PythonInterface* oif, QObject* parent)
    : QObject(parent), m_oif(oif)
{
    QObject::connect(m_oif, &PythonInterface::relatedCallerExited,
                     [=](QPointer<Caller> caller) {
                         if (!m_relClientRemovedHandle.isNull()) {
                             PyCaller* thePyCaller = new PyCaller(caller);
                             thePyCaller->deleteLater();
                             m_relClientRemovedHandle.call(
                                 QVariantList()
                                 << QVariant::fromValue(thePyCaller));
                         }
                     });
}

QString PyObj::objectName() const
{
    return m_oif->objectName();
}

void PyObj::mutexLock()
{
    m_oif->mutexLock();
}

void PyObj::mutexUnlock()
{
    m_oif->mutexUlock();
}

void PyObj::setPrivateData(const NS_BGMRPCObjectInterface::PyCaller* caller,
                           const QString& key, const QVariant& value)
{
    if (caller && caller->online())
        m_oif->setPrivateData(caller->caller(), key, value);
}

QVariant PyObj::privateData(const NS_BGMRPCObjectInterface::PyCaller* caller,
                            const QString& key) const
{
    if (caller && caller->online())
        return m_oif->privateData(caller->caller(), key);
    else
        return QVariant();
}

QVariant
PyObj::callLocalMethod(const NS_BGMRPCObjectInterface::PyCaller* caller,
                       const QString& object, const QString& method,
                       const QVariantList& args)
{
    return m_oif->callLocalMethod(caller->caller(), object, method, args);
}

/*void PyObj::callLocalMethodNonblock(
    const NS_BGMRPCObjectInterface::PyCaller* caller, const QString& object,
    const QString& method, const QVariantList& args)
{
    qDebug() << "callLocalMethodNonblock 1";
    return m_oif->callLocalMethodNonblock(caller->caller(), object, method,
                                          args);
}*/

QVariant PyObj::callLocalMethod(const QString& object, const QString& method,
                                const QVariantList& args)
{
    return m_oif->callLocalMethod(object, method, args);
}

void PyObj::callLocalMethodNonblock(const QString& object,
                                    const QString& method,
                                    const QVariantList& args)
{
    return m_oif->callLocalMethodNonblock(object, method, args);
}

bool PyObj::addRelClient(const NS_BGMRPCObjectInterface::PyCaller* caller)
{
    //    PyCaller* thePyCaller = caller.value<PyCaller*>();
    if (caller && caller->online()) {
        m_oif->addRelatedCaller(caller->caller());
        return true;
    } else
        return false;
}

bool PyObj::removeRelClient(const NS_BGMRPCObjectInterface::PyCaller* caller)
{
    //    PyCaller* thePyCaller = caller.value<PyCaller*>();
    if (caller && caller->online())
        return m_oif->removeRelatedCaller(caller->caller());
    else
        return false;
}

bool PyObj::removeRelClient(qint64 callerID)
{
    return removeRelClient(relClient(callerID));
}

QVariantList PyObj::relClients() const
{
    QVariantList cliList;

    m_oif->findRelatedCaller([&](QPointer<Caller> caller) -> bool {
        PyCaller* thePyCaller = new PyCaller(caller);
        thePyCaller->deleteLater();
        cliList << QVariant::fromValue(thePyCaller);

        return false;
    });

    return cliList;
}

NS_BGMRPCObjectInterface::PyCaller* PyObj::relClient(qint64 callerID) const
{
    QPointer<Caller> caller =
        m_oif->findRelatedCaller([&](QPointer<Caller> caller) -> bool {
            if (caller->ID() == callerID)
                return true;
            else
                return false;
        });

    if (!caller.isNull()) {
        PyCaller* thePyCaller = new PyCaller(caller);
        thePyCaller->deleteLater();

        return thePyCaller;
    }

    return nullptr;
}

bool PyObj::containsRelClient(
    const NS_BGMRPCObjectInterface::PyCaller* caller) const
{
    if (caller && caller->online())
        return !m_oif
                    ->findRelatedCaller(
                        [=](QPointer<Caller> theCaller) -> bool {
                            if (theCaller->ID() == caller->ID())
                                return true;
                            else
                                return false;
                        })
                    .isNull();
    else
        return false;
}

bool PyObj::containsRelClient(qint64 callerID) const
{
    return containsRelClient(relClient(callerID));
}

NS_BGMRPCObjectInterface::PyCaller*
PyObj::findRelClient(PythonQtObjectPtr callback)
{
    QPointer<Caller> targetCaller =
        m_oif->findRelatedCaller([&](QPointer<Caller> theCaller) -> bool {
            PyCaller* thePyCaller = new PyCaller(theCaller);
            thePyCaller->deleteLater();
            return callback
                .call(QVariantList() << QVariant::fromValue(thePyCaller))
                .toBool();
        });

    if (targetCaller.isNull())
        return nullptr;
    else {
        PyCaller* targetPyCaller = new PyCaller(targetCaller);
        targetPyCaller->deleteLater();
        return targetPyCaller;
    }
}

void PyObj::onRelClientRemoved(PythonQtObjectPtr callback)
{
    m_relClientRemovedHandle = callback;
}

void PyObj::emitSignal(const QString& signal, const QVariantList& args)
{
    m_oif->emitSignal(signal, args);
}
