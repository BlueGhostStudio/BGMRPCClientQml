#ifndef CHATPLUGIN_H
#define CHATPLUGIN_H

#include <bgmrobject.h>
#include <bgmradaptor.h>
#include <relatedproc.h>
#include <QtCore>

#include "chatplugin_global.h"

using namespace BGMircroRPCServer;

class chatObj;
class chatAdaptor;

class chatObj : public QObject, public BGMRObject < chatAdaptor >
{
    Q_OBJECT
public:
    chatObj (QObject* parent = 0);
    QString objectType () const;
    QJsonArray say (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray join (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray hasJoined (BGMRProcedure* p, const QJsonArray&);
    QJsonArray changeNickname (BGMRProcedure* p, const QJsonArray& args);
    QJsonArray whoList (BGMRProcedure*p, const QJsonArray&);
    QJsonArray leave (BGMRProcedure* p, const QJsonArray&);

public slots:
    void leaved (BGMRProcedure* proc);

private:
    relatedProcs RelProc;

    QString join (BGMRProcedure* p, const QString& nick = QString ());
    bool hasJoined (BGMRProcedure* p);
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
