#ifndef MATHPLUS_H
#define MATHPLUS_H

#include "mathplus_global.h"
#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <bgmrobjectstorage.h>

using namespace BGMircroRPCServer;
class mathObj;
class mathAdaptor;
//class BGMRPC;

class math
{
public:
    double plus (double a, double b);
    double minus (double a, double b);
};

class mathObj : public BGMRObject < mathAdaptor >
{
public:
    QString objectType () const;
    QJsonArray plus (BGMRProcedure*, const QJsonArray& args);
    QJsonArray minus (BGMRProcedure*, const QJsonArray& args);

private:
    math Math;
};

class mathAdaptor : public BGMRAdaptor < mathObj >
{
public:
    mathAdaptor () { registerMethods (); }

    void registerMethods ();
};

extern "C" {
BGMRObjectInterface* objCreator ();
QString objType ();
bool initial(BGMRObjectStorage* storage, BGMRPC* rpc); // NOTE 测试
}

#endif // MATHPLUS_H
