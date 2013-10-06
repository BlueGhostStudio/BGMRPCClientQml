#include "bgmrpc.h"
#include "bgmrobject.h"
#include "bgmradaptor.h"
#include "bgmrtcpserver.h"
#include "bgmrobjectstorage.h"
#include <QDir>

namespace BGMircroRPCServer {
QSettings* BGMRPC::Settings = new QSettings ("BG", "BGMR");

BGMRPC::BGMRPC()
{
    initial ();
}

BGMRPC::~BGMRPC ()
{
    delete __RPCTcpServer;
    delete objectStorage ();
}

void BGMRPC::initial()
{
    __RPCTcpServer = new BGMRTcpServer (this);
    ObjectStorage = new BGMRObjectStorage (this);
}

/*BGMRClientConnectionsManager*
BGMRPC::clientConnectionsManager() const
{
    return ClientConnectionsManager;
}*/

BGMRTcpServer*
BGMRPC::RPCTcpServer() const
{
    return __RPCTcpServer;
}

BGMRObjectStorage* BGMRPC::objectStorage() const
{
    return ObjectStorage;
}

BGMRObjectInterface*
BGMRPC::object(const QString& objName) const
{
    BGMRObjectInterface* theObj = ObjectStorage->object (objName);
    if (!theObj)
        qCritical () << QObject::tr ("The Object %1 does not exist.").arg (objName);

    return theObj;
}

void BGMRPC::setSettings(const QString& fileName)
{
    delete Settings;
    Settings = new QSettings (fileName, QSettings::NativeFormat);
}

//QSettings*BGMRPC::settings() const
//{
//    return Settings;
//}

//bool BGMRPC::installPlugin(const QString& pluginFileName)
//{
//    QString path = QString ("%1%2")
//                     .arg(QString(qgetenv ("BGMRPCPLUGIN")))
//                     .arg (pluginFileName);
//    QLibrary plugin (path);
//    if (plugin.load ()) {
//        objCreator_T creator = (objCreator_T)plugin.resolve ("objCreator");
//        QString (*objType)() = (QString (*)())plugin.resolve ("objType");
//        if (creator && objType)
//            ObjectStorage->installCreator (objType (),
//                                           creator);
//        else
//            return false;
//    } else
//        return false;

//    return true;
//}

}
