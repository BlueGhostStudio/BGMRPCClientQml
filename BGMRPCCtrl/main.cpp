#include <bgmrpccommon.h>
#include <getopt.h>

#include <QCoreApplication>
#include <QDebug>
#include <QJsonDocument>
#include <QLocalSocket>
#include <QProcess>
#include <QSettings>
#include <QTimer>
#include <functional>

bool serverRunning = false;
QLocalSocket ctrlSocket;
QString binPath;

bool runApp(const QString& app, const QString& grp,
            const QString& pApp = QString());

void
startServer(int argc, char* argv[]) {
    if (serverRunning) {
        qWarning().noquote() << "Server already running";
        return;
    }

    QSettings* settings;
    QSettings defaultSettings(defaultEtcDir + "/BGMRPC.conf",
                              QSettings::IniFormat);
    //    QString startCmd;
    QStringList args;
    QString rootPath;
    QString optPath;
    QString logPath;
    QString binPath;

    QProcess BGMRPCProcess;

    if (argc >= 3) {
        args << "-c" << argv[2];
        settings = new QSettings(argv[2], QSettings::IniFormat);
    } else {
        settings = new QSettings();
    }

    optPath =
#ifdef REMOTEPATH
        REMOTEPATH;
#else
        QDir::homePath();
#endif

    rootPath = settings
                   ->value("path/root",
                           defaultSettings.value("path/root", QDir::homePath()))
                   .toString();
    rootPath.replace(QRegularExpression("^~"), QDir::homePath());

    binPath = settings
                  ->value("path/bin",
                          defaultSettings.value("path/bin", optPath + "/bin"))
                  .toString();

    BGMRPCProcess.setProgram(binPath + "/BGMRPCd");
    BGMRPCProcess.setArguments(args);
    BGMRPCProcess.startDetached();

    QThread::msleep(1000);

    ctrlSocket.connectToServer(BGMRPCServerCtrlSocket);
    if (ctrlSocket.waitForConnected(500)) serverRunning = true;

    runApp("AutoStart", "", "");
}

void
stopServer() {
    if (serverRunning) {
        QByteArray cmd(1, NS_BGMRPC::CTRL_STOPSERVER);
        qInfo().noquote() << "BGMRPC,stopServer,Stoping...";
        ctrlSocket.write(cmd);
        if (ctrlSocket.waitForBytesWritten())
            qInfo().noquote() << "BGMRPC,stopServer,Stoped";
    }
}

bool
checkObject(const QByteArray& objName) {
    QByteArray checkObject(1, NS_BGMRPC::CTRL_CHECKOBJECT);
    checkObject.append(objName);
    ctrlSocket.write(checkObject);
    if (!ctrlSocket.waitForBytesWritten() || !ctrlSocket.waitForReadyRead()) {
        qWarning().noquote()
            << "Object,createObject-checkObject,Can't check object";
        return false;
    } else if ((quint8)ctrlSocket.readAll()[0] == 1) {
        qWarning().noquote() << QString(
                                    "Object,createObject-checkObject,The "
                                    "Object(%1) already existed")
                                    .arg(objName);
        return true;
    } else
        return false;
}

bool
createObject(const QByteArray& name, const QStringList& args) {
    if (checkObject(name)) return true;

    QProcess loaderProcess;
    QString binPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_BIN);

    loaderProcess.setProgram(binPath + (QT_VERSION > 0X060000
                                            ? "/BGMRPCObjectLoader"
                                            : "/BGMRPCObjectLoader-qt5"));
    loaderProcess.setArguments(args);
    loaderProcess.startDetached();

    bool ok = false;
    QThread::msleep(1000);
    ok = checkObject(name);

    qInfo().noquote() << "CreateObject"
                      << "(" + name + ")" << (ok ? "ok" : "fail");

    return ok;
}

bool
createObject(int argc, char* argv[]) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,createObject,Server not run";
        return false;
    } else if (argc < 4) {
        qWarning().noquote() << "Object,createObject,Mistake arguments";
        return false;
    }

    QByteArray app;
    QByteArray group;
    bool noAppPrefix = false;
    int opt = 0;
    while ((opt = getopt(argc, argv, "g:a:A")) != -1) {
        switch (opt) {
        case 'g':
            group = optarg;
            break;
        case 'a':
            app = optarg;
            break;
        case 'A':
            noAppPrefix = true;
            break;
        case '?':
            break;
        }
    }

    QByteArray objName = genObjectName(group, app, argv[2], noAppPrefix);

    QStringList args;
    for (int i = 4; i < argc; i++) args << argv[i];

    return createObject(objName, args);
}

bool
detachObject(const QByteArray& obj) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,detachObject,server not run";
        return false;
    }

    qInfo().noquote()
        << QString("Object(%1),detachObject,Detach object...").arg(obj);

    QByteArray cmd(1, NS_BGMRPC::CTRL_DETACHOBJECT);
    cmd.append(obj);
    ctrlSocket.write(cmd);
    if (ctrlSocket.waitForReadyRead() && (quint8)ctrlSocket.readAll()[0])
        qInfo().noquote()
            << QString("Object(%1),detachObject,Finished").arg(obj);
    else
        qWarning().noquote()
            << QString("Object(%1),detachObject,Detach Fail").arg(obj);

    return true;
}

bool
detachObject(int argc, char* argv[]) {
    if (argc < 2) {
        qWarning().noquote() << "Object,detachObject,Mistake arguments";
        return false;
    }

    return detachObject(argv[2]);
}

void
listObjects() {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,listObjects,Server not run";
        return;
    }

    QByteArray cmd(1, NS_BGMRPC::CTRL_LISTOBJECTS);
    ctrlSocket.write(cmd);
    if (!ctrlSocket.waitForBytesWritten() || !ctrlSocket.waitForReadyRead()) {
        qWarning().noquote() << "BGMRPC,listObjects,Can't get object list";
        return;
    }

    QByteArray objListData = ctrlSocket.readAll();
    if (objListData[0] != '\x0') {
        foreach (const QByteArray obj, objListData.split(','))
            qInfo().noquote() << "- " << obj;
    }
}

void
listApps() {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC,listObjects,Server not run";
        return;
    }

    QByteArray rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);
    if (rootPath.isEmpty()) return;

    QDir appsDir(rootPath + "/apps");
    foreach (const QString& app,
             appsDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        qInfo().noquote() << "- " << app;
    }
}

int
iteratorRelApps(
    const QString& grp, const QJsonArray& jsaRelApps,
    std::function<bool(const QString&, const QString&, const QJsonValue&)> cb) {
    foreach (const QJsonValue& jsvRelApp, jsaRelApps) {
        QString relAppName;
        QString relAppGrp = grp;

        if (jsvRelApp.isObject()) {
            QJsonObject jsoRelApp = jsvRelApp.toObject();
            relAppName = jsoRelApp["app"].toString("");

            if (jsoRelApp.contains("grp")) {
                relAppGrp = jsoRelApp["grp"].toString();
                if (relAppGrp.isEmpty()) relAppGrp = grp;
            }
        } else
            relAppName = jsvRelApp.toString("");

        if (relAppName.isEmpty()) continue;

        if (!cb(relAppName, relAppGrp, jsvRelApp)) return false;
    }

    return true;
}

bool
processAppJson(
    const QString& app, const QString& grp,
    std::function<bool(const QString&, const QString&, const QJsonValue&)>
        requiredAppsCB,
    std::function<bool(const QString&, const QString&, const QJsonValue&)>
        optionalAppsCB,
    std::function<bool(const QString&, const QString&, bool,
                       const QJsonObject&)>
        objectCB,
    bool required) {
    QByteArray rootPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT);
    if (rootPath.isEmpty()) return false;

    QDir appDir(rootPath + "/apps/" + app);
    if (!appDir.exists() || !appDir.exists("app.json")) return false;

    QFile appJsonFile(appDir.filePath("app.json"));
    if (!appJsonFile.open(QIODevice::ReadOnly)) return false;

    QByteArray jsonData = appJsonFile.readAll();
    QJsonDocument appJson = QJsonDocument::fromJson(jsonData);
    appJsonFile.close();

    QString group = grp.isEmpty() ? appJson["grp"].toString("") : grp;

    if (!iteratorRelApps(group, appJson["required-apps"].toArray({}),
                         requiredAppsCB) &&
        required)
        return false;

    iteratorRelApps(group, appJson["optional-apps"].toArray({}),
                    optionalAppsCB);

    QJsonArray jsaObjs = appJson["objs"].toArray({});
    foreach (const QJsonValue& jsvObj, jsaObjs) {
        QJsonObject jsoObj = jsvObj.toObject({});

        QString objName = jsoObj["obj"].toString("");
        if (objName.isEmpty()) continue;

        if (!objectCB(group, objName, jsoObj["noprefix"].toBool(false),
                      jsoObj) &&
            required)
            return false;
    }
    return true;
}

bool
genArgs(const QJsonObject& IFTypes, const QJsonObject& jsoObj,
        QStringList& args) {
    if (IFTypes.isEmpty()) return false;

    const QString& IFName(jsoObj["IF"].toString(""));
    if (IFName.isEmpty() || !IFTypes.contains(IFName)) return false;

    QJsonObject jsoIFType = IFTypes[IFName].toObject();

    QJsonObject::const_iterator it;
    bool resetIF = false;
    for (it = jsoObj.constBegin(); it != jsoObj.constEnd(); ++it) {
        if (!jsoIFType.contains(it.key())) continue;

        if (it.key() == "IF") {
            args << "-I" << jsoIFType["IF"].toString();
            resetIF = true;
        } else {
            QJsonObject jsoOpt = jsoIFType[it.key()].toObject();

            if (jsoOpt["hasArg"].toBool(false)) {
                QString value = it.value().toString("");
                if (!value.isEmpty())
                    args << jsoOpt["opt"].toString() << it.value().toString();
            } else if (it.value().toBool(false))
                args.prepend(jsoOpt["opt"].toString());
        }
    }

    return resetIF;
}

bool
runApp(const QString& app, const QString& grp, const QString& pApp) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC, Server not run";
        return false;
    }

    QJsonObject IFTypes;

    auto loadIFTypes = [&](const QByteArray& etcDir) -> bool {
        QString IFTypesFileName = etcDir + "/IF_types.json";
        bool ok = false;
        if (QFile::exists(IFTypesFileName)) {
            QFile IFTypesFile(IFTypesFileName);
            if (IFTypesFile.open(QIODevice::ReadOnly)) {
                QJsonDocument IFTypesDoc =
                    QJsonDocument::fromJson(IFTypesFile.readAll());
                ok = !IFTypesDoc.isNull();
                if (ok) IFTypes = IFTypesDoc.object();
            }
        }

        return ok;
    };
    if (!loadIFTypes(getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ETC)))
        loadIFTypes(getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_ROOT) + "/etc");

    qInfo().noquote() << "Starting " + app + " ...";

    processAppJson(
        app, grp,
        [app](const QString& relApp, const QString& relAppGrp,
              const QJsonValue&) -> bool {
            return runApp(relApp, relAppGrp, app);
        },
        [app](const QString& relApp, const QString& relAppGrp,
              const QJsonValue&) -> bool {
            runApp(relApp, relAppGrp, app);
            return true;
        },
        [app, IFTypes, pApp](const QString& grp, const QString& objName,
                             bool noprefix, const QJsonObject& jsoObj) -> bool {
            QString interface = jsoObj["IF"].toString("");
            if (interface.isEmpty()) return false;

            QString IFPath = jsoObj["IFPath"].toString("");

            // QStringList args({ "-n", objName, "-a", app });
            QStringList args;
            if (!IFPath.isEmpty())
                args << "-p" << IFPath;

            args << "-n" << objName;

            QString fullAppName;  //= pApp.isEmpty() ? app : pApp + "::" + app;

            if (noprefix) {
                fullAppName = app;
                args << "-A";
            } else
                fullAppName = pApp.isEmpty() ? app : pApp + "::" + app;

            args << "-a" << fullAppName;

            if (!grp.isEmpty()) args << "-g" << grp;

            args << QProcess::splitCommand(jsoObj["args"].toString(""));

            QStringList IFTypeArgs;

            if (!genArgs(IFTypes, jsoObj, IFTypeArgs))
                args << "-I" << interface;
            if (!IFTypeArgs.isEmpty()) args << IFTypeArgs;

            return createObject(
                genObjectName(grp.toLatin1(), fullAppName.toLatin1(),
                              objName.toLatin1(), noprefix),
                args);

            return true;
        },
        true);

    qDebug().noquote() << "\n\n";

    return true;
}

bool
stopApp(const QString& app, const QString& grp,
        const QString& pApp = QString()) {
    if (!serverRunning) {
        qWarning().noquote() << "BGMRPC, listObjects, Server not run";
        return false;
    }

    qInfo().noquote() << "Stopping " + app + " ...";

    QString fullAppName = pApp.isEmpty() ? app : pApp + "::" + app;

    auto stopRelApps = [fullAppName](const QString& relApp,
                                     const QString& relAppGrp,
                                     const QJsonValue& jsvRelApp) -> bool {
        if (!jsvRelApp.isObject() ||
            !jsvRelApp.toObject({})["skip-stop"].toBool(false)) {
            stopApp(relApp, relAppGrp, fullAppName);
        }

        return true;
    };

    processAppJson(
        app, grp, stopRelApps, stopRelApps,
        [fullAppName](const QString& grp, const QString& objName, bool noprefix,
                      const QJsonObject& jsoObj) -> bool {
            detachObject(genObjectName(grp.toLatin1(), fullAppName.toLatin1(),
                                       objName.toLatin1(), noprefix));
            return true;
        },
        false);

    qDebug().noquote() << "\n\n";

    return true;
}

int
main(int argc, char* argv[]) {
    QCoreApplication::setSetuidAllowed(true);

    QCoreApplication a(argc, argv);

    a.setApplicationName("BGMRPC Ctrl");
#ifdef VERSION
    a.setApplicationVersion(VERSION);
#else
    a.setApplicationVersion("1.0.0");
#endif

    a.setOrganizationName("BlueGhost Studio");
    a.setOrganizationDomain("bgstudio.org");

    QString usage =
        "commands: start, stop, createObject, detachObject, "
        "listObjects, listApps, runApp, stopApp";

    if (argc < 2) {
        qWarning().noquote() << usage;
        return -1;
    }

    ctrlSocket.connectToServer(BGMRPCServerCtrlSocket);

    if (ctrlSocket.waitForConnected(500)) serverRunning = true;

    if (serverRunning)
        binPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_BIN);

    if (strcmp(argv[1], "start") == 0)
        startServer(argc, argv);
    else if (strcmp(argv[1], "stop") == 0)
        stopServer();
    else if (strcmp(argv[1], "createObject") == 0)
        createObject(argc, argv);
    else if (strcmp(argv[1], "detachObject") == 0)
        detachObject(argc, argv);
    else if (strcmp(argv[1], "listObjects") == 0)
        listObjects();
    else if (strcmp(argv[1], "listApps") == 0)
        listApps();
    else if (strcmp(argv[1], "runApp") == 0 && argc > 2)
        runApp(argv[2], argc > 3 ? argv[3] : "");
    else if (strcmp(argv[1], "stopApp") == 0 && argc > 2)
        stopApp(argv[2], argc > 3 ? argv[3] : "");
    else
        qWarning().noquote() << usage;
}
