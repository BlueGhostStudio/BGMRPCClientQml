#ifndef RELATEDPROC_H
#define RELATEDPROC_H

#include <QObject>
#include <bgmrclient.h>

namespace BGMircroRPCServer {

class BGMRPCSHARED_EXPORT relatedClients : public QObject
{
    Q_OBJECT
public:
    explicit relatedClients(QObject *parent = 0);

    bool addClient(BGMRClient* cli);
    QMap<qulonglong, BGMRClient*> clients () const;
    BGMRClient* client (qulonglong cliID) const;
    void emitSignal (BGMRObjectInterface* obj, const QString& signal,
                     const QJsonArray& args) const;

signals:
    void removedClient (BGMRClient* p);

public slots:
    bool removeClient (qulonglong id);
    
protected:
    QMap < qulonglong, BGMRClient* > Clients;
};

}
#endif // RELATEDPROC_H
