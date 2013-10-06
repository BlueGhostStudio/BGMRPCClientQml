#ifndef BGMROBJECT_H
#define BGMROBJECT_H
#include "bgmrpc_global.h"
#include <QJsonArray>
#include "bgmrprocedure.h"

namespace BGMircroRPCServer {

class BGMRAdaptorInterface;
class BGMRProcedure;

class BGMRObjectInterface
{
public:
    virtual BGMRAdaptorInterface* adaptor () = 0;
    virtual void setObjectName (const QString& objName) = 0;
    virtual QString objectName () const = 0;
    virtual QString objectType () const = 0;
    virtual bool procIdentify (BGMRProcedure*, const QJsonObject& call) = 0;
};

template < typename T >
class BGMRObject : public BGMRObjectInterface {
public:
//    BGMRObject (BGMRPC* rpc) : RPC (rpc) {}
    BGMRAdaptorInterface* adaptor () { return AdaptorRef; }
    QString objectName () const { return ObjectName; }
    void setObjectName (const QString& objName) { ObjectName = objName; }
    bool procIdentify (BGMRProcedure*, const QJsonObject&)
    {
        return true;
    }


protected:
    static T* AdaptorRef;
    QString ObjectName;
};

template < typename T >
T* BGMRObject < T >::AdaptorRef = new T ();
}
#endif // BGMROBJECT_H
