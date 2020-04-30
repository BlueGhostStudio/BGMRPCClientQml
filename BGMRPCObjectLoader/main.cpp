#include <QCoreApplication>
#include <QDebug>
#include <QLibrary>
#include <getopt.h>
#include <objectinterface.h>

int main(int argc, char* argv[]) {
    QCoreApplication a(argc, argv);
    qSetMessagePattern("%{time process} %{if-debug}[D]%{function}:%{line} - "
                       "%{endif}%{if-warning}[W]%{function}:%{line} - "
                       "%{endif}%{if-critical}[C]%{function}:%{line} - "
                       "%{endif}%{if-fatal}[F]%{function}:%{line} - "
                       "%{endif}%{if-info}[INFO]%{endif}%{message}");
    /*NS_BGMRPCObjectInterface::ObjectInterface objIF;
    objIF.registerObject("test1");
    QObject::connect(&objIF,
                     &NS_BGMRPCObjectInterface::ObjectInterface::objectDisconnected,
    [&] {
        a.quit();
    });*/
    /*NS_BGMRPCObjectInterface::ObjectImplement objImp;
    objImp.registerObject("testLocalCall");
    QObject::connect(
        &objImp, &NS_BGMRPCObjectInterface::ObjectInterface::objectDisconnected,
        [&]() { a.quit(); });*/

    QString remoteObjectName;
    int opt = 0;
    while ((opt = getopt(argc, argv, "kn:")) != -1) {
        switch (opt) {
        case 'k':
            qDebug() << "kill";
            break;
        case 'n':
            remoteObjectName = optarg;
            break;
        }
    }
    QString libName = argv[optind];

    qInfo() << "Starting Remote object. The name is" << remoteObjectName
            << ". By" << libName;

    QLibrary IFLib(libName);
    if (IFLib.load())
        qInfo() << "Load interface library - OK";

    typedef NS_BGMRPCObjectInterface::ObjectInterface* (*T_CREATE)();
    T_CREATE create = (T_CREATE)IFLib.resolve("create");
    NS_BGMRPCObjectInterface::ObjectInterface* objIF = create();
    objIF->registerObject(remoteObjectName.toLatin1());
    QObject::connect(
        objIF, &NS_BGMRPCObjectInterface::ObjectInterface::objectDisconnected,
        [&] { a.quit(); });

    return a.exec();
}
