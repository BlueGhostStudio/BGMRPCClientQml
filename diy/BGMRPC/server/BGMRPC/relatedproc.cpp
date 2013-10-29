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
                 this, SLOT(removeProc(qulonglong)), Qt::DirectConnection);
        ok = true;
    }

    return ok;
}

QMap<qulonglong, BGMRProcedure*> relatedProcs::procs() const
{
    return Procs;
}

BGMRProcedure* relatedProcs::proc(qulonglong pID) const
{
    if (Procs.contains (pID))
        return Procs [pID];
    else
        return NULL;
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
    bool ok = false;
    if (Procs.contains (id)) {
        ok = true;
        BGMRProcedure* p = Procs [id];
        disconnect (p, 0, this, 0);
        emit removedProc (p);
        Procs.remove (id);
    }

    return ok;
}

}
