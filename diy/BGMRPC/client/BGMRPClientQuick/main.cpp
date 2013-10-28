#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include <QtQuick>
#include "bgmrpcc.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
    BGMRPCC thisBGMRPCC (viewer.engine ());
    viewer.rootContext ()->setContextProperty ("BGMRPC", &thisBGMRPCC);
    viewer.rootContext ()->setContextProperty ("cliQml", argv [1]);
    viewer.setMainQmlFile(QStringLiteral("qml/BGMRPCallQuick/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
