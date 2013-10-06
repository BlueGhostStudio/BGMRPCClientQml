#include "jsglobalobject.h"

jsRPCObject::jsRPCObject(BGMRObjectInterface* obj)
    : Object (obj)
{
}

QString jsRPCObject::objectName() const
{
    return Object->objectName ();
}

QString jsRPCObject::objectType() const
{
    return Object->objectType ();
}

QStringList jsRPCObject::objectMethods() const
{
    return Object->adaptor ()->methods ();
}



jsGlobalObject::jsGlobalObject(BGMRObjectInterface* obj) :
    jsRPCObject (obj)
{
}

void jsGlobalObject::addProc(BGMRProcedure* proc)
{
    thisRelProcs ()->addProc (proc);
}

bool jsGlobalObject::removeProc(qulonglong pID)
{
    return thisRelProcs ()->removeProc (pID);
}

relProcsMap jsGlobalObject::relProcs() const
{
    return thisRelProcs ()->procs ();
}

BGMRProcedure*jsGlobalObject::relProc(qulonglong pID) const
{
    return thisRelProcs ()->proc (pID);
}

void jsGlobalObject::emitSignal(const QString& obj, const QString& signal, const QJsonValue& args)
{
    thisRelProcs ()->emitSignal (obj, signal, args.toArray ());
}

relatedProcs*jsGlobalObject::thisRelProcs() const
{
    return static_cast < jsObj * > (Object)->relProcs ();
}
