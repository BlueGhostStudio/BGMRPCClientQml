#ifndef JSGLOBALOBJECT_H
#define JSGLOBALOBJECT_H

#include <QObject>
#include "jsplugin.h"

using namespace BGMircroRPCServer;

class jsRPCObject : public QObject
{
    Q_PROPERTY(QString __NAME__ READ objectName)
    Q_PROPERTY(QString __TYPE__ READ objectType)
    Q_PROPERTY(QStringList __METHODS__ READ objectMethods)
    Q_OBJECT
public:
    explicit jsRPCObject (BGMRObjectInterface* obj);

    Q_INVOKABLE QString objectName () const;
    Q_INVOKABLE QString objectType () const;
    Q_INVOKABLE QStringList objectMethods () const;

protected:
    BGMRObjectInterface* Object;
};

class jsGlobalObject : public jsRPCObject
{
    Q_OBJECT
public:
    explicit jsGlobalObject(BGMRObjectInterface* obj);

    Q_INVOKABLE void addProc(BGMRProcedure* proc);
    Q_INVOKABLE bool removeProc (qulonglong pID);
    Q_INVOKABLE relProcsMap relProcs () const;
    Q_INVOKABLE BGMRProcedure* relProc (qulonglong pID) const;
    Q_INVOKABLE void emitSignal (const QString& obj,
                                 const QString& signal,
                                 const QJsonValue& args);

private:
    relatedProcs* thisRelProcs () const;
};

#endif // JSGLOBALOBJECT_H
