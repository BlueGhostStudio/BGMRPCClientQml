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
    QJsonArray say (BGMRClient* p, const QJsonArray& args);
    QJsonArray join (BGMRClient* p, const QJsonArray& args);
    QJsonArray hasJoined (BGMRClient* p, const QJsonArray&);
    QJsonArray changeNickname (BGMRClient* p, const QJsonArray& args);
    QJsonArray whoList (BGMRClient*p, const QJsonArray&);
    QJsonArray leave (BGMRClient* p, const QJsonArray&);

public slots:
    void leaved (BGMRClient* proc);

private:
    relatedClients RelProc;

    QString join (BGMRClient* p, const QString& nick = QString ());
    bool hasJoined (BGMRClient* p);
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
