#include "relatedclient.h"


namespace BGMircroRPCServer {

relatedClients::relatedClients(QObject *parent) :
    QObject(parent)
{
}


bool relatedClients::addClient(BGMircroRPCServer::BGMRClient* cli)
{
    bool ok = false;
    if (cli) {
        Clients [cli->cliID ()] = cli;
        connect (cli, SIGNAL(clientExited(qulonglong)),
                 this, SLOT(removeClient(qulonglong)), Qt::DirectConnection);
        ok = true;
    }

    return ok;
}

QMap<qulonglong, BGMRClient*> relatedClients::clients() const
{
    return Clients;
}

BGMRClient* relatedClients::client(qulonglong cliID) const
{
    if (Clients.contains (cliID))
        return Clients [cliID];
    else
        return NULL;
}

void relatedClients::emitSignal(BGMRObjectInterface* obj,
                             const QString& signal,
                             const QJsonArray& args) const
{
    QMap < qulonglong, BGMRClient* >::const_iterator it;
    for (it = Clients.constBegin (); it != Clients.constEnd (); ++it)
        it.value ()->emitSignal (obj, signal, args);
}

bool relatedClients::removeClient(qulonglong id)
{
    bool ok = false;
    if (Clients.contains (id)) {
        ok = true;
        BGMRClient* p = Clients [id];
        disconnect (p, 0, this, 0);
        emit removedClient (p);
        Clients.remove (id);
    }

    return ok;
}

}
