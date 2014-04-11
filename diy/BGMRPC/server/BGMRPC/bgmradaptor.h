#ifndef BGMRADAPTOR_H
#define BGMRADAPTOR_H

#include "bgmrpc_global.h"
#include "bgmrobject.h"
#include <QThread>

namespace BGMircroRPCServer {

class BGMRProcedure;
class BGMRAdaptorInterface
{
public:
    BGMRAdaptorInterface() {}

    virtual void registerMethods () = 0;
    virtual QJsonArray callMetchod (BGMRObjectInterface* obj,
                                    BGMRProcedure* proc,
                                    const QString& method,
                                    const QJsonArray& args) = 0;
    virtual QStringList methods () const = 0;
};

template < typename T >
class BGMRAdaptor : public BGMRAdaptorInterface
{
public:
    QJsonArray callMetchod (BGMRObjectInterface* obj,
                            BGMRProcedure* proc,
                            const QString& method,
                            const QJsonArray& args);
    QStringList methods () const;

protected:
    QMap < QString, QJsonArray (T::*)(BGMRProcedure* proc, const QJsonArray&) > Methods;
};

template < typename T >
QJsonArray BGMRAdaptor<T>::callMetchod (BGMRObjectInterface* obj,
                                        BGMRProcedure* proc,
                                        const QString& method,
                                        const QJsonArray& args)
{
    QJsonArray ret;
    T* theObj = static_cast < T* > (obj);
    if (theObj && Methods.contains (method)) {
        if (theObj->procIdentify (proc, method, args))
            ret = (theObj->*Methods [method])(proc, args);
        else {
            QJsonArray sigArgs;
            sigArgs.append (method);
            for (int i = 0; i < args.count (); i++)
                sigArgs.append (args [i].toString ());
            proc->emitSignal (theObj, "ERROR_ACCESS", sigArgs);
        }
    } else {
        if (!theObj)
            qCritical () << QObject::tr ("Object %1 unable to be converted.").arg (obj->objectName ());
        else
            qCritical () << QObject::tr ("Object %1 does not exist %2 method.").arg (obj->objectName ()).arg (method);

        ret = QJsonArray ();
    }
    return ret;
}

template < typename T >
QStringList BGMRAdaptor < T >::methods() const
{
    return Methods.keys ();
}

}

#endif // BGMRADAPTOR_H
