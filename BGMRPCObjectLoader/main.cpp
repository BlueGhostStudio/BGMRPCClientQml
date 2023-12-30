#include <bgmrpccommon.h>
#include <getopt.h>
#include <grp.h>
#include <objectinterface.h>
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QElapsedTimer>
#include <QLibrary>

/*QElapsedTimer timer;
quint8 messageFlag = 0x1f;
void OIFMessage(QtMsgType type, const QMessageLogContext& context,
                const QString& msg)
{
    const char* logMsg = msg.toLocal8Bit().constData();
    const char* function = context.function ? context.function : "";
    switch (type) {
    case QtMsgType::QtDebugMsg:
        if (messageFlag & 0x01)
            fprintf(stdout, "%lld \033[0m[DEBUG] %s:%u - %s\033[0m\n",
                    timer.elapsed(), function, context.line, logMsg);
        break;
    case QtMsgType::QtInfoMsg:
        if (messageFlag & 0x02)
            fprintf(stdout, "%lld \033[34m[INFO]\033[0m %s\n", timer.elapsed(),
                    logMsg);
        break;
    case QtMsgType::QtWarningMsg:
        if (messageFlag & 0x04)
            fprintf(stderr, "%lld \033[31m[WARNING] %s:%u - %s\033[0m\n",
                    timer.elapsed(), function, context.line, logMsg);
        break;
    case QtMsgType::QtCriticalMsg:
        if (messageFlag & 0x08)
            fprintf(stderr, "%lld \033[31m[CRITICAL] %s:%u - %s\033[0m\n",
                    timer.elapsed(), function, context.line, logMsg);
        break;
    case QtMsgType::QtFatalMsg:
        if (messageFlag & 0x10)
            fprintf(stderr, "%lld \033[31m[FATAL] %s:%u - %s\033[0m\n",
                    timer.elapsed(), function, context.line, logMsg);
        break;
    }
}*/

int
main(int argc, char* argv[]) {
    // FINISHED interfacePath, rootPath未获取设定值
    QCoreApplication::setSetuidAllowed(true);
    QLocalSocket ctrlSocket;
    ctrlSocket.connectToServer(BGMRPCServerCtrlSocket);
    if (!ctrlSocket.waitForConnected(-1)) {
        qWarning() << QString("Connect to BGMRPC Fail");
        return -1;
    }

    QString interfacesPath =
        getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_INTERFACES);
    QString rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);

    initialLogMessage(/*0x1D*/);

    QCoreApplication a(argc, argv);

    a.setApplicationName("BGMRPC ObjectLoader");
#ifdef VERSION
    a.setApplicationVersion(VERSION);
#else
    a.setApplicationVersion("1.0.0");
#endif
    a.setOrganizationName("BlueGhost Studio");
    a.setOrganizationDomain("bgstudio.org");

    QByteArray remoteObjectName;
    QString libName;
    QByteArray appGroup;
    QByteArray app;
    bool noAppPrefix = false;
    int opt = 0;
    opterr = 0;
    while ((opt = getopt(argc, argv, "g:a:An:I:p:")) != -1) {
        switch (opt) {
        case 'g':
            appGroup = optarg;
            break;
        case 'a':
            app = optarg;
            break;
        case 'A':
            noAppPrefix = true;
            break;
        case 'n':
            remoteObjectName = optarg;
            break;
        case 'p':
            interfacesPath = optarg;
            break;
        case 'I':
            libName = optarg;
            break;
        case '?':
            break;
        }
    }

    if (app.isEmpty()) app = remoteObjectName;

    qInfo().noquote()
        << QString("ObjectLoader,startRemoteObject,The name is %1. by %2")
               .arg(remoteObjectName, libName);
    libName = interfacesPath + '/' + libName;
    if (app.isEmpty())
        QDir::setCurrent(rootPath);
    else
        QDir::setCurrent(rootPath + "/apps/" + app);

    QLibrary IFLib(libName);
    IFLib.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    if (IFLib.load()) {
        qInfo().noquote() << QString(
                                 "ObjectLoader,loadInterface,"
                                 "Load interface library(%1) - OK")
                                 .arg(libName);

        bool ok = false;

        QByteArray userName = getSettings(ctrlSocket, "access/user");
        if (userName.isEmpty()) userName = "nobody";
        QByteArray groupName = getSettings(ctrlSocket, "access/group");
        if (groupName.isEmpty()) groupName = "nobody";

        int uid = -1;
        int gid = -1;

        passwd* usrInfo = getpwnam(userName);
        group* grpInfo = getgrnam(groupName);

        if (!usrInfo || !grpInfo || setgid(grpInfo->gr_gid) != 0 ||
            setuid(usrInfo->pw_uid) != 0)
            qWarning().noquote() << "Permission denied";
        // TODO test access
        else {
            typedef NS_BGMRPCObjectInterface::ObjectInterface* (*T_CREATE)();
            T_CREATE create = (T_CREATE)IFLib.resolve("create");
            NS_BGMRPCObjectInterface::ObjectInterface* objIF = create();
            ok = objIF->setup(app, remoteObjectName, appGroup, argc, argv,
                              noAppPrefix);
            if (!ok) delete objIF;
        }

        ctrlSocket.close();

        if (ok)
            return a.exec();
        else
            return -1;
    }

    // Read some settings
    /*QString interfacesPath;
    QString rootPath;

    QLocalSocket ctrlSocket;
    ctrlSocket.connectToServer(BGMRPCServerCtrlSocket);

    if (!ctrlSocket.waitForConnected(500)) return -1;

    QByteArray cmd(1, NS_BGMRPC::CTRL_DAEMONCTRL);
    ctrlSocket.write(cmd);
    if (!ctrlSocket.waitForBytesWritten()) return -1;
    if (ctrlSocket.waitForReadyRead())
        ctrlSocket.readAll();
    else
        return -1;

    interfacesPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_INTERFACES);
    rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);

    // initial Log message
    initialLogMessage();

    QCoreApplication a(argc, argv);

    QString remoteObjectName;
    QString libName;
    QString group;
    QString app;
    int opt = 0;
    opterr = 0;
    while ((opt = getopt(argc, argv, "g:a:n:I:p:")) != -1) {
        switch (opt) {
        case 'g':
            group = optarg;
            break;
        case 'a':
            app = optarg;
            break;
        case 'n':
            remoteObjectName = optarg;
            break;
        case 'p':
            interfacesPath = optarg;
            break;
        case 'I':
            libName = optarg;
            break;
        case '?':
            break;
        }
    }

    if (app.isEmpty()) app = remoteObjectName;

    qInfo().noquote()
        << QString("ObjectLoader,startRemoteObject,The name is %1. by %2")
               .arg(remoteObjectName)
               .arg(libName);
    // 13288999788
    libName = interfacesPath + '/' + libName;
    if (app.isEmpty())
        QDir::setCurrent(rootPath);
    else
        QDir::setCurrent(rootPath + "/apps/" + app);

    QLibrary IFLib(libName);
    IFLib.setLoadHints(QLibrary::ExportExternalSymbolsHint);
    if (IFLib.load()) {
        qInfo().noquote()
            << QString(
                   "ObjectLoader,loadInterface,Load interface library(%1) - OK")
                   .arg(libName);

        typedef NS_BGMRPCObjectInterface::ObjectInterface* (*T_CREATE)();
        T_CREATE create = (T_CREATE)IFLib.resolve("create");
        NS_BGMRPCObjectInterface::ObjectInterface* objIF =
            create();
        objIF->registerObject(app.toLatin1(), remoteObjectName.toLatin1(),
                              group.toLatin1());

        if (group.isEmpty()) group = "default";
        objIF->initial(rootPath + "/apps/" + app,
                       rootPath + "/data/" + group + '/' + app, argc, argv);

        QObject::connect(
            objIF,
            &NS_BGMRPCObjectInterface::ObjectInterface::objectDisconnected, &a,
            &QCoreApplication::quit);

        return a.exec();
    } else
        qWarning().noquote()
            << "ObjectLoader,loadInterface,Can't load interface library"
            << libName << ". " << IFLib.errorString();*/
}
