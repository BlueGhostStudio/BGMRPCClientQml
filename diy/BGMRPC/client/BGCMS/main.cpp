#include "mainwindow.h"
#include <QApplication>
#include <bgmrpcclient.h>
#include <QFile>
#include <QDebug>

BGMRPCClient* RPC;
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QIcon winIcon ("://icons/appIcon.png");
    qDebug () << winIcon.actualSize (QSize(512,512));
    QApplication::setWindowIcon (winIcon);

    QFile styleSheetFile ("/home/bgstudio/Mis/stylesheet");
    styleSheetFile.open (QIODevice::ReadOnly);
    a.setStyleSheet (styleSheetFile.readAll ());

    RPC = new BGMRPCClient ();
    RPC->setUrl ("ws://127.0.0.1", 8000);
    RPC->connectToHost();

    RPC->setProperty ("CliID", RPC->callMethod ("CMS", "js", {"clientID"}));


    RPC->callMethod("CMS", "js", { "login", "bgstudio", "123456" });

    MainWindow w;
    w.show();

    return a.exec();
}
