#include "bgmrpccommon.h"

#include <QElapsedTimer>

QByteArray BGMRPCObjPrefix = "BGMRPC_OBJ_";
QByteArray BGMRPCServerCtrlSocket = "BGMRPC_CTRL";
// QByteArray BGMRPCObjectCtrlSocket = "BGMRPC_OBJ_CTRL";
QByteArray BGMRPCObjectSocket = "BGMRPC_OBJSOCKET";

bool
splitLocalSocketFragment(QLocalSocket* socket,
                         std::function<void(const QByteArray&)> callback) {
    int lenLen = sizeof(quint64);
    bool end = false;
    while (socket->bytesAvailable()) {
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
    }

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

void
initialLogMessage(quint8 mf) {
    static quint8 messageFlag = mf;
    timer.start();
    qInstallMessageHandler([](QtMsgType type, const QMessageLogContext& context,
                              const QString& msg) {
        const char* logMsg = msg.toLocal8Bit().constData();
        const char* function = context.function ? context.function : "";
        switch (type) {
        case QtMsgType::QtDebugMsg:
            if (messageFlag & 0x01)
                fprintf(stdout, "%10.3f,DEBUG,%s:%u,%s\n",
                        (double)timer.elapsed() / 1000, function, context.line,
                        logMsg);
            break;
        case QtMsgType::QtInfoMsg:
            if (messageFlag & 0x02)
                fprintf(stdout, "%10.3f,INFO,%s\n",
                        (double)timer.elapsed() / 1000, logMsg);
            break;
        case QtMsgType::QtWarningMsg:
            if (messageFlag & 0x04)
                fprintf(stderr, "%10.3f,WARNING,%s:%u,%s\n",
                        (double)timer.elapsed() / 1000, function, context.line,
                        logMsg);
            break;
        case QtMsgType::QtCriticalMsg:
            if (messageFlag & 0x08)
                fprintf(stderr, "%10.3f,CRITICAL,%s:%u,%s\n",
                        (double)timer.elapsed() / 1000, function, context.line,
                        logMsg);
            break;
        case QtMsgType::QtFatalMsg:
            if (messageFlag & 0x10)
                fprintf(stderr, "%10.3f,FATAL,%s:%u,%s\n",
                        (double)timer.elapsed() / 1000, function, context.line,
                        logMsg);
            break;
        }
    });
}
