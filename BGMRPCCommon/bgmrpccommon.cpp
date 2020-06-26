#include "bgmrpccommon.h"
#include <QElapsedTimer>

// namespace NS_BGMRPC {
QString BGMRPCObjPrefix = "BGMRPC_OBJ_";
QString BGMRPCCtrlSocket = "BGMRPC_CTRL";

void splitData(const QByteArray& data,
                     std::function<void(const QByteArray&)> callback)
{
    if (!callback)
        return;

    int i = 0;
    int lenLen = sizeof(quint64);
    while (i < data.length()) {
        quint64 len = bytes2int<quint64>(data.mid(i, lenLen));
        i += lenLen;
        QByteArray retData = data.mid(i, len);
        i += len;
        callback(retData);
    }
}

//} // namespace NS_BGMRPC

QByteArray getSettings(QLocalSocket& ctrlSocket, NS_BGMRPC::Config cnf)
{
    QByteArray cmd(2, '\x0');
    cmd[0] = NS_BGMRPC::CTRL_CONFIG;
    cmd[1] = cnf;

    ctrlSocket.write(cmd);
    if (ctrlSocket.waitForReadyRead())
        return ctrlSocket.readAll();
    else
        return QByteArray();
}

QElapsedTimer timer;

void initialLogMessage(quint8 mf)
{
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
