#ifndef RELATEDPROC_H
#define RELATEDPROC_H

#include <QObject>
#include <bgmrprocedure.h>

namespace BGMircroRPCServer {

class BGMRPCSHARED_EXPORT relatedProcs : public QObject
{
    Q_OBJECT
public:
    explicit relatedProcs(QObject *parent = 0);

    bool addProc(BGMRProcedure* proc);
    QMap<qulonglong, BGMRProcedure*> procs () const;
    BGMRProcedure* proc (qulonglong pID) const;
    void emitSignal (BGMRObjectInterface* obj, const QString& signal,
                     const QJsonArray& args) const;

public slots:
    bool removeProc (qulonglong id);
    
protected:
    QMap < qulonglong, BGMRProcedure* > Procs;
};

}
#endif // RELATEDPROC_H
