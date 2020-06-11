#include <QCoreApplication>
#include <QDebug>
#include <QLocalSocket>
#include <QProcess>
#include <QSettings>
#include <bgmrpccommon.h>
#include <string>

bool serverRunning = false;
QLocalSocket ctrlSocket;
QString binPath;

void startServer(int argc, char* argv[])
{
    if (serverRunning) {
        qWarning().noquote() << "Server already running";
        return;
    }

    QSettings* settings;
    //    QString startCmd;
    QStringList args;
    QString rootPath;
    QString logPath;
    QString binPath;

    QProcess BGMRPCProcess;

    if (argc >= 3) {
        args << "-c" << argv[2];
        settings = new QSettings(argv[2], QSettings::IniFormat);
    } else {
        settings = new QSettings();
    }

    rootPath = settings->value("path/root").toString();
    rootPath.replace(QRegExp("^~"), QDir::homePath());
    logPath = settings->value("path/logs", rootPath + "/logs").toString() +
              "/BGMRPC.log";
    logPath.replace(QRegExp("^~"), QDir::homePath());
    binPath = settings->value("path/bin", rootPath + "/bin").toString();
    binPath.replace(QRegExp("^~"), QDir::homePath());

    if (qgetenv("BGMRPCDebug") != "1") {
        BGMRPCProcess.setStandardOutputFile(logPath /*, QIODevice::Append*/);
        BGMRPCProcess.setStandardErrorFile(logPath /*, QIODevice::Append*/);
    }
    BGMRPCProcess.setProgram(binPath + "/BGMRPCd");
    BGMRPCProcess.setArguments(args);
    BGMRPCProcess.startDetached();
}

void stopServer()
{
    if (serverRunning) {
        QByteArray cmd;
        cmd[0] = NS_BGMRPC::CTRL_STOPSERVER;
        qInfo().noquote() << "Stoping...";
        ctrlSocket.write(cmd);
        if (ctrlSocket.waitForBytesWritten())
            qInfo().noquote() << "Stoped";
    }
}

void createObject(int argc, char* argv[])
{
    if (!serverRunning) {
        qWarning().noquote() << "Server not run";
        return;
    } else if (argc < 4) {
        qWarning().noquote() << "Mistake arguments";
        return;
    }

    QByteArray cmd(1, NS_BGMRPC::CTRL_CHECKOBJECT);
    cmd.append(argv[2]);
    ctrlSocket.write(cmd);
    if (!ctrlSocket.waitForBytesWritten() || !ctrlSocket.waitForReadyRead()) {
        qWarning().noquote() << "Can't check object";
        return;
    }

    if ((quint8)ctrlSocket.readAll()[0] == 1) {
        qWarning().noquote() << "The Object already existed";
        return;
    }

    QStringList args;
    args << "-n" << argv[2] << "-I" << argv[3];
    for (int i = 4; i < argc; i++)
        args << argv[i];

    QProcess loaderProcess;
    QString logPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_LOGS) + "/" +
                      QString(argv[2]) + ".log";
    QString binPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_BIN);

    qDebug() << logPath << binPath;
    if (qgetenv("BGMRPCDebug") != "1") {
        loaderProcess.setStandardOutputFile(logPath /*, QIODevice::Append*/);
        loaderProcess.setStandardErrorFile(logPath /*, QIODevice::Append*/);
    }
    loaderProcess.setProgram(binPath + "/BGMRPCObjectLoader");
    loaderProcess.setArguments(args);
    loaderProcess.startDetached();
}

void stopObject(int argc, char* argv[])
{
    if (!serverRunning) {
        qWarning().noquote() << "server not run";
        return;
    } else if (argc < 2) {
        qWarning().noquote() << "Mistake arguments";
        return;
    }

    QByteArray cmd;
    cmd[0] = NS_BGMRPC::CTRL_DETACHOBJECT;
    qInfo().noquote() << "Detach object...";
    cmd.append(argv[2]);
    ctrlSocket.write(cmd);
    if (ctrlSocket.waitForReadyRead() && (quint8)ctrlSocket.readAll()[0])
        qInfo().noquote() << "Finished";
    else
        qWarning().noquote() << "Fail";
}

void listObjects()
{
    if (!serverRunning) {
        qWarning().noquote() << "Server not run";
        return;
    }

    QByteArray cmd(1, NS_BGMRPC::CTRL_LISTOBJECTS);
    ctrlSocket.write(cmd);
    if (!ctrlSocket.waitForBytesWritten() || !ctrlSocket.waitForReadyRead()) {
        qWarning().noquote() << "Can't get object list";
        return;
    }

    QByteArray objListData = ctrlSocket.readAll();
    if (objListData[0] != '\x0')
        foreach (QByteArray obj, objListData.split(','))
            qInfo().noquote() << "    " << obj;
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    QString usage =
        "commands: start, stop, createObject, detachObject, listObjects";

    if (argc < 2) {
        qWarning().noquote() << usage;
        return -1;
    }

    ctrlSocket.connectToServer(BGMRPCCtrlSocket);

    if (ctrlSocket.waitForConnected(500))
        serverRunning = true;

    if (serverRunning) {
        QByteArray cmd;
        cmd[0] = NS_BGMRPC::CTRL_DAEMONCTRL;
        ctrlSocket.write(cmd);
        if (!ctrlSocket.waitForBytesWritten())
            return -1;

        if (ctrlSocket.waitForReadyRead())
            ctrlSocket.readAll();
        else
            return -1;

        binPath = getSettings(ctrlSocket, NS_BGMRPC::CNF_PATH_BIN);
    }

    if (strcmp(argv[1], "start") == 0)
        startServer(argc, argv);
    else if (strcmp(argv[1], "stop") == 0)
        stopServer();
    else if (strcmp(argv[1], "createObject") == 0)
        createObject(argc, argv);
    else if (strcmp(argv[1], "detachObject") == 0)
        stopObject(argc, argv);
    else if (strcmp(argv[1], "listObjects") == 0)
        listObjects();
    else
        qWarning().noquote() << usage;
}
