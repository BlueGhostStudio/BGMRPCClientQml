#ifndef BGMROBJECT_H
#define BGMROBJECT_H
#include "bgmrpc_global.h"
#include <QJsonArray>
#include "bgmrclient.h"

namespace BGMircroRPCServer {

class BGMRAdaptorInterface;
class BGMRClient;

class BGMRObjectInterface
{
public:
    virtual ~BGMRObjectInterface () {}
    virtual BGMRAdaptorInterface* adaptor () = 0;
    virtual void setObjectName (const QString& objName) = 0;
    virtual QString objectName () const = 0;
    virtual QString objectType () const = 0;
    virtual bool clientIdentify (BGMRClient*, const QString&,
                               const QJsonArray&) = 0;
    virtual void destory () = 0;
    QJsonArray callMethod (BGMRClient* cli, const QString& method,
                           const QJsonArray& args);
};

template < typename T >
class BGMRObject : public BGMRObjectInterface {
public:
    BGMRAdaptorInterface* adaptor () { return AdaptorRef; }
    QString objectName () const { return ObjectName; }
    void setObjectName (const QString& objName) { ObjectName = objName; }
    void destory () { qDebug () << "Destoring " << objectName (); }
    bool clientIdentify (BGMRClient*, const QString&, const QJsonArray&)
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
