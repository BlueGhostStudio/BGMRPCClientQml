#include "mainwindow.h"
#include <QApplication>
#include <bgmrpcclient.h>
#include <QFile>
#include <QDebug>
#include <QWebEngineSettings>
#include <cmssettings.h>

BGMRPCClient* RPC;
int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("BGStudio");
    QCoreApplication::setOrganizationDomain("bg.studio");
    QCoreApplication::setApplicationName("BGCMS");

    /*QWebEngineSettings* wevSettings
            = QWebEngineSettings::defaultSettings ();
    QString defaultWevFont
            = wevSettings->fontFamily (QWebEngineSettings::StandardFont);

    QSettings settings;
    wevSettings->setFontFamily (
                QWebEngineSettings::StandardFont,
                settings.value ("previewfont", defaultWevFont).toString ());*/
    QString wevFontFamily = CMSSettings::previewFontFamily ();
    QWebEngineSettings::defaultSettings ()
            ->setFontFamily (QWebEngineSettings::StandardFont,
                             wevFontFamily);

    QIcon winIcon ("://icons/appIcon.png");
    QApplication::setWindowIcon (winIcon);

    QFile styleSheetFile ("/home/bgstudio/Mis/stylesheet");
    styleSheetFile.open (QIODevice::ReadOnly);
    a.setStyleSheet (styleSheetFile.readAll ());

    RPC = new BGMRPCClient ();
    QUrl cmsUrl = CMSSettings::CMSUrl ();
    if (!cmsUrl.isEmpty ()) {
        RPC->setUrl (cmsUrl);
        RPC->connectToHost();
    }
    RPC->setProperty ("CliID", RPC->callMethod ("CMS", "js", {"clientID"}));

    MainWindow w;
    w.show();

    RPC->callMethod("CMS", "js", {
                        "login",
                        CMSSettings::user (),
                        CMSSettings::password ()
                    });

    return a.exec();
}
