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

    QString defaultIFPath =
        getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_INTERFACES);
    QString appIFPath;
    QString rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);

    // initialLogMessage(/*0x1D*/);

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
            appIFPath = optarg;
            break;
        case 'I':
            libName = optarg;
            break;
        case '?':
            break;
        }
    }

    if (app.isEmpty()) app = remoteObjectName;

    QString logPath = getSettings(ctrlSocket, "path/logs");
    if (logPath.isEmpty()) logPath = rootPath + "/logs";
    logPath += QString("/%1.log").arg(
        genObjectName(appGroup, app, remoteObjectName, noAppPrefix));
    initialLogMessage(logPath);

    qInfo().noquote()
        << QString("ObjectLoader,startRemoteObject,The name is %1. by %2")
               .arg(remoteObjectName, libName);

    QString libPath;

    if (!QFile::exists(libPath = appIFPath + "/lib" + libName + ".so") &&
        !QFile::exists(libPath = rootPath + "/apps/" + app + "/lib" + libName +
                                 ".so"))
        libPath = defaultIFPath + "/lib" + libName + ".so";

    if (app.isEmpty())
        QDir::setCurrent(rootPath);
    else
        QDir::setCurrent(rootPath + "/apps/" + app);

    QLibrary IFLib(libPath);
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
}
