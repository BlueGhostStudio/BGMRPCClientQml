#include <bgmrpccommon.h>
#include <getopt.h>

#include <QCoreApplication>
#include <QSettings>

#include "bgmrpc.h"

int
main(int argc, char* argv[]) {
    QCoreApplication::setSetuidAllowed(true);

    initialLogMessage();
    QCoreApplication a(argc, argv);

    a.setApplicationName("BGMRPC");
    a.setApplicationVersion("2.0.0");
    a.setOrganizationName("BlueGhost Studio");
    a.setOrganizationDomain("bgstudio.org");

    int opt = 0;
    QString settingFile;

    while ((opt = getopt(argc, argv, "c:")) != -1) {
        switch (opt) {
        case 'c':
            settingFile = optarg;
            break;
        }
    }

    /*QSettings* settings;

    if (settingFile.isEmpty())
        settings = new QSettings();
    else
        settings = new QSettings(settingFile, QSettings::IniFormat);*/

    NS_BGMRPC::BGMRPC RPC;

    RPC.initial(settingFile);

    /*QString ip = settings->value("server/ip").toString();
    RPC.setAddress(!ip.isEmpty() ? QHostAddress(ip) : QHostAddress::Any);
    RPC.setPort(settings->value("server/port", 8000).toInt());*/

    RPC.start();

    return a.exec();
}
