#include "bgmrobject.h"
#include "bgmradaptor.h"

namespace BGMircroRPCServer {

QJsonArray BGMRObjectInterface::callMethod(BGMRClient* cli,
                                           const QString& method,
                                           const QJsonArray& args)
{
    QJsonArray ret = adaptor()->callMetchod(this, cli, method, args);

    return ret;
}

}
