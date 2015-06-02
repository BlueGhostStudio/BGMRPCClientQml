#ifndef BGMRPC_H
#define BGMRPC_H

#include "bgmrpc_global.h"
#include <QSettings>

namespace BGMircroRPCServer {

class BGMRObjectInterface;
class BGMRTcpServer;
class BGMRObjectStorage;

class BGMRPCSHARED_EXPORT BGMRPC
{

public:
    BGMRPC();
    ~BGMRPC ();

    void initial ();

    //BGMRClientConnectionsManager* clientConnectionsManager () const;
    BGMRTcpServer* RPCTcpServer () const;
    BGMRObjectStorage* objectStorage () const;
    BGMRObjectInterface* object (const QString& objName) const;
    void destoryAllObject ();
//    void setSettings (const QString& fileName);
//    QSettings* settings () const;
    //bool installPlugin (const QString& pluginFileName);
    static QSettings* Settings;
    static void setSettings (const QString& fileName);

private:
    //BGMRClientConnectionsManager* ClientConnectionsManager;
    BGMRTcpServer* __RPCTcpServer;
    BGMRObjectStorage* ObjectStorage;
    QList < QLibrary > LoadedPlus;  // 保存加载的lib，以便过后的清理工作
//    QSettings* Settings;
};

}
#endif // BGMRPC_H
