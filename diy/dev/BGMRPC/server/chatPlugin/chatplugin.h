#ifndef CHATPLUGIN_H
#define CHATPLUGIN_H

#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <relatedproc.h>

#include "chatplugin_global.h"

using namespace BGMircroRPCServer;

class chatObj;
class chatAdaptor;

class chatObj : public BGMRObject < chatAdaptor >
{
public:
    QString objectType () const;
    QJsonArray say (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray join (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray changeNickname (BGMRProcedure* p, const QJsonArray& args);

private:
    relatedProcs RelProc;

    QString join (BGMRProcedure* p, const QString& nick = QString ());
    bool hasJoin (BGMRProcedure* p);
};

class chatAdaptor : public BGMRAdaptor < chatObj >
{
public:
    chatAdaptor () { registerMethods (); }
    void registerMethods ();
};

extern "C" {
BGMRObjectInterface* objCreator ();
QString objType ();
}

#endif // CHATPLUGIN_H
