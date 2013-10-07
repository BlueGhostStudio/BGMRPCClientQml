#include "relatedproc.h"


namespace BGMircroRPCServer {

relatedProcs::relatedProcs(QObject *parent) :
    QObject(parent)
{
}


bool relatedProcs::addProc(BGMircroRPCServer::BGMRProcedure* proc)
{
    bool ok = false;
    if (proc) {
        Procs [proc->pID ()] = proc;
        connect (proc, SIGNAL(procExited(qulonglong)),
                 this, SLOT(removeProc(qulonglong)));
        ok = true;
    }

    return ok;
}

QMap<qulonglong, BGMRProcedure*> relatedProcs::procs() const
{
    return Procs;
}

BGMRProcedure*relatedProcs::proc(qulonglong pID) const
{
    return Procs [pID];
}

void relatedProcs::emitSignal(BGMRObjectInterface* obj,
                             const QString& signal,
                             const QJsonArray& args) const
{
    QMap < qulonglong, BGMRProcedure* >::const_iterator it;
    for (it = Procs.constBegin (); it != Procs.constEnd (); ++it)
        it.value ()->emitSignal (obj, signal, args);
}

bool relatedProcs::removeProc(qulonglong id)
{
    return Procs.remove (id) > 0 ? true : false;
}

}
