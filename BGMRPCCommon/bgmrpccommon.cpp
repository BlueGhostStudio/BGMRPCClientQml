#include "bgmrpccommon.h"

#include <QElapsedTimer>
#include <QFile>

QByteArray BGMRPCObjPrefix = "BGMRPC_OBJ_";
QByteArray BGMRPCServerCtrlSocket = "BGMRPC_CTRL";
// QByteArray BGMRPCObjectCtrlSocket = "BGMRPC_OBJ_CTRL";
QByteArray BGMRPCObjectSocket = "BGMRPC_OBJSOCKET";

#ifdef DEFAULT_ETC_DIR
QString defaultEtcDir = DEFAULT_ETC_DIR;
#else
QString defaultEtcDir = "/usr/share/BGStudio/BGMRPC/etc";
#endif

bool
splitLocalSocketFragment(QLocalSocket* socket,
                         std::function<void(const QByteArray&)> callback) {
    int lenLen = sizeof(quint64);
    bool end = false;

    quint64 len = 0;
    quint64 readLen = 0;
    QByteArray readData;

    if (socket->property("fragment").isValid()) {
        len = socket->property("dataLen").toULongLong();
        readData = socket->property("fragment").toByteArray();
        readLen = readData.length();
    }
    while (socket->bytesAvailable()) {
        if (len == 0) len = bytes2int<quint64>(socket->read(lenLen));

        readData += socket->read(len - readLen);
        readLen = readData.length();

        if (readLen == len) {
            callback(readData);
            readData.clear();
            len = 0;
            readLen = 0;
            end = true;
        }
    }

    socket->setProperty("dataLen", len == 0 ? QVariant() : len);
    socket->setProperty("fragment", len == 0 ? QVariant() : readData);
    /*while (socket->bytesAvailable()) {
        quint64 len = 0;
        quint64 readLen = 0;
        QByteArray readData;
        if (!socket->property("fragment").isValid()) {
            len = bytes2int<quint64>(socket->read(lenLen));
            socket->setProperty("dataLen", len);
        } else {
            len = socket->property("dataLen").toULongLong();
            readData = socket->property("fragment").toByteArray();
            readLen = readData.length();
        }

        readData += socket->read(len - readLen);
        readLen = readData.length();

        if (readLen < len) {
            socket->setProperty("fragment", readData);
            //                break;
        } else {
            callback(readData);
            socket->setProperty("fragment", QVariant());
            socket->setProperty("dataLen", QVariant());
            end = true;
        }
    }*/

    return end;
}

QByteArray
getSettings(QLocalSocket& ctrlSocket, NS_BGMRPC::Config cnf) {
    QByteArray cmd(2, '\x0');
    cmd[0] = NS_BGMRPC::CTRL_CONFIG;
    cmd[1] = cnf;

    ctrlSocket.write(cmd);
    if (ctrlSocket.waitForReadyRead())
        return ctrlSocket.readAll();
    else
        return QByteArray();
}

QByteArray
getSettings(QLocalSocket& ctrlSocket, const QByteArray& key) {
    QByteArray cmd(1, NS_BGMRPC::CTRL_SETTING);
    cmd.append(key);

    ctrlSocket.write(cmd);
    if (ctrlSocket.waitForReadyRead())
        return ctrlSocket.readAll();
    else
        return QByteArray();
}

QElapsedTimer timer;
int logOutput = 1;
QFile logFile;
QString logFileName;

void
initialLogMessage(const QString& logFilePath, quint8 mf) {
    logFile.setFileName(logFilePath);

    logFileName = QFileInfo(logFilePath).baseName();
    if (logFileName.length() > 15)
        logFileName = "..." + logFileName.last(12);
    else
        logFileName = logFileName.leftJustified(15, ' ');

    if (logFile.open(QIODevice::Append)) logOutput = logFile.handle();

    static quint8 messageFlag = mf;
    timer.start();
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context,
                              const QString& msg) {
        QString flag;
        // const char* logMsg = std::move(msg.toLocal8Bit().constData());
        // const char* function = context.function ? context.function : "";

        switch (type) {
        case QtMsgType::QtDebugMsg:
            if (messageFlag & 0x01) flag = "D";
            break;
        case QtMsgType::QtInfoMsg:
            if (messageFlag & 0x02) flag = "I";
            break;
        case QtMsgType::QtWarningMsg:
            if (messageFlag & 0x04) flag = "W";
            break;
        case QtMsgType::QtCriticalMsg:
            if (messageFlag & 0x08) flag = "C";
            break;
        case QtMsgType::QtFatalMsg:
            if (messageFlag & 0x10) flag = "F";
            break;
        }
        dprintf(logOutput, "%10.3f <<%s>> [%s] %s\n",
                (double)timer.elapsed() / 1000,
                logFileName.toLatin1().constData(), flag.toLatin1().constData(),
                msg.toLatin1().constData());
    });
    logFile.flush();
}

QByteArray
genObjectName(const QByteArray& grp, const QByteArray& app,
              const QByteArray& name, bool noAppPrefix) {
    if (noAppPrefix)
        return name;
    else if (!grp.isEmpty())
        return grp + "::" + app + "::" + name;
    else
        return (!app.isEmpty() ? app + "::" : "") + name;
}

bool
checkObject(QLocalSocket* ctrlSocket, const QString& objName) {
    return ctrlCommand<bool>(
        ctrlSocket, (quint8)NS_BGMRPC::CTRL_CHECKOBJECT, objName.toLatin1(),
        [](const QByteArray& data) -> bool { return (quint8)data[0]; });
}
