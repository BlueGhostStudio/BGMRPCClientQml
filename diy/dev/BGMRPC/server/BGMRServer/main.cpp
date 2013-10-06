#include <QCoreApplication>
#include <bgmrpc.h>
#include <bgmrtcpserver.h>
#include <bgmrobjectstorage.h>
#include <bgmrobject.h>
#include <QDir>
#include <QtGlobal>
#include <getopt.h>

#define PLUGINPATH "BGMRPCPLUGIN"


using namespace BGMircroRPCServer;

BGMRPC theRPCServer;

void initialPlugin (/*QString& pluginDir*/)
{
    QString pluginDir (BGMRPC::Settings->value ("pluginDir",
                                                "~/.BGMR/plugin").toString ());

    pluginDir.remove (QRegExp ("/\\s*$"));
    BGMRObjectStorage* theObjectStorage = theRPCServer.objectStorage ();
    //theObjectStorage->setPluginDir (pluginDir);

    QDir autoloadPluginDir (QString ("%1/autoload").arg (pluginDir));
    QFileInfoList pluginFiles = autoloadPluginDir.entryInfoList (QStringList () << "*.so");
    foreach (QFileInfo pluginFile, pluginFiles) {
        QString pluginFileName (pluginFile.fileName ());
        if (!theObjectStorage->installPlugin ("autoload/" + pluginFileName))
            qCritical () << QObject::tr ("installPlugin error - %1").arg (pluginFileName);
        else
            qDebug () << QObject::tr ("installPlugin ok - %1").arg (pluginFileName);
    }
}

int main(int argc, char *argv[])
{
    int opt;
    bool startServer = true;
    quint16 port = 8000;
    QString pluginDir;
    QString rootDir;
    QHostAddress address = QHostAddress::Any;
    while ((opt = getopt (argc, argv, "p:P:r:s:a:")) != -1) {
        QString optVal (optarg);
        switch (opt) {
        case 'p':
            port = optVal.toUInt ();
            break;
        case 'P':
            pluginDir = optVal;
            startServer = false;
            break;
        case 'r':
            rootDir = optVal;
            startServer = false;
            break;
        case 's':
            BGMRPC::setSettings (optVal);
            break;
        case 'a':
            address.setAddress (optVal);
            break;
        default:
            qDebug () << "Invalid option." << endl;
            exit (0);
        }
    }

    if (!pluginDir.isEmpty ())
        BGMRPC::Settings->setValue ("pluginDir", pluginDir);
    if (!rootDir.isEmpty ())
        BGMRPC::Settings->setValue ("rootDir", rootDir);

    if (startServer) {
//        QString pluginDir (theSettings.value ("pluginDir",
//                                              "~/.BGMR/plugin").toString ());
//        QString rootDir (theSettings->value ("rootDir",
//                                            "~/.BGMR").toString ());
        QCoreApplication a(argc, argv);

        QString QT_PLUGIN_PATH = qgetenv ("QT_PLUGIN_PATH");
        foreach (QString pluginPath, QT_PLUGIN_PATH.split (':'))
            QCoreApplication::addLibraryPath (pluginPath);

        QString locale = QLocale::system().name();

        QTranslator BGMRTrans;
        BGMRTrans.load(QString("BGMRPC_") + locale);
        a.installTranslator(&BGMRTrans);
        QTranslator serTrans;
        serTrans.load (QString("serverDemo_") + locale);
        a.installTranslator (&serTrans);

        initialPlugin (/*pluginDir*/);
//        QDir::setCurrent (rootDir);
        BGMRTcpServer* server = theRPCServer.RPCTcpServer ();
        server->setPort (port);
        if (server->activeServer (address))
            qDebug () << QObject::tr ("server has started.");
        else
            qCritical () << QObject::tr ("server no start.");

        return a.exec();
    } else
        return 0;
}
