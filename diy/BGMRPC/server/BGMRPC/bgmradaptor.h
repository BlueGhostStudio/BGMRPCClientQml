#ifndef BGMRADAPTOR_H
#define BGMRADAPTOR_H

#include "bgmrpc_global.h"
#include "bgmrobject.h"
#include <QThread>

namespace BGMircroRPCServer {

class BGMRClient;
class BGMRAdaptorInterface
{
public:
    BGMRAdaptorInterface() {}

    virtual void registerMethods () = 0;
    virtual QJsonArray callMetchod (BGMRObjectInterface* obj,
                                    BGMRClient* cli,
                                    const QString& method,
                                    const QJsonArray& args) = 0;
    virtual QStringList methods () const = 0;
};

template < typename T >
class BGMRAdaptor : public BGMRAdaptorInterface
{
public:
    QJsonArray callMetchod (BGMRObjectInterface* obj,
                            BGMRClient* cli,
                            const QString& method,
                            const QJsonArray& args);
    QStringList methods () const;

protected:
    QMap < QString, QJsonArray (T::*)(BGMRClient* cli, const QJsonArray&) > Methods;
};

template < typename T >
QJsonArray BGMRAdaptor<T>::callMetchod (BGMRObjectInterface* obj,
                                        BGMRClient* cli,
                                        const QString& method,
                                        const QJsonArray& args)
{
    QJsonArray ret;
    T* theObj = static_cast < T* > (obj);
    if (theObj && Methods.contains (method)) {
        if (theObj->clientIdentify (cli, method, args))
            ret = (theObj->*Methods [method])(cli, args);
        else {
            QJsonArray sigArgs;
            sigArgs.append (method);
            for (int i = 0; i < args.count (); i++)
                sigArgs.append (args [i].toString ());
            cli->emitSignal (theObj, "ERROR_ACCESS", sigArgs);
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
