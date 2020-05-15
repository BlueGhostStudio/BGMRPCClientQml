#include <QCoreApplication>
#include <QDebug>
#include <QElapsedTimer>
#include <QLibrary>
#include <getopt.h>
#include <objectinterface.h>
#include <stdio.h>
#include <stdlib.h>

QElapsedTimer timer;
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
}

int main(int argc, char* argv[])
{
    timer.start();

    QCoreApplication a(argc, argv);
    //    messageFlag = 0x1c;
    qInstallMessageHandler(OIFMessage);

    QString remoteObjectName;
    QString libName;
    int opt = 0;
    opterr = 0;
    while ((opt = getopt(argc, argv, "kn:l:")) != -1) {
        switch (opt) {
        case 'k':
            qDebug() << "kill";
            break;
        case 'n':
            remoteObjectName = optarg;
            break;
        case 'l':
            libName = optarg;
            break;
        case '?':
            qDebug() << "some other args";
            break;
        }
    }

    qInfo().noquote() << "Starting Remote object. The name is"
                      << remoteObjectName << ". By" << libName;

    QLibrary IFLib(libName);
    if (IFLib.load())
        qInfo().noquote() << "Load interface library - OK";

    typedef NS_BGMRPCObjectInterface::ObjectInterface* (*T_CREATE)(int, char**);
    T_CREATE create = (T_CREATE)IFLib.resolve("create");
    NS_BGMRPCObjectInterface::ObjectInterface* objIF = create(argc, argv);
    objIF->registerObject(remoteObjectName.toLatin1());

    return a.exec();
}
