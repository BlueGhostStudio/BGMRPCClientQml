#include <QtGui/QGuiApplication>
#include "qtquick2applicationviewer.h"
#include <QtQuick>
#include "bgmrpc.h"
#include "clifiles.h"
#include "proc.h"
#include "step.h"

QJSValue callStepJs;
QQmlEngine* engine;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    qmlRegisterType < BGMRPC > ("BGMRPC", 1, 0, "RPC");
    qmlRegisterType < proc > ("BGMRPC", 1, 0, "Proc");
//    qmlRegisterType < step > ("BGMRpc", 1, 0, "Step");
    qmlRegisterUncreatableType < step > ("BGMrpc", 1, 0, "step", "");
    qmlRegisterType < _RStep > ("BGMRPC", 1, 0, "RStep");
    qmlRegisterType < _JSStep > ("BGMRPC", 1, 0, "JSStep");
    qmlRegisterType < _LStep > ("BGMRPC", 1, 0, "LStep");

    QtQuick2ApplicationViewer viewer;
    engine = viewer.engine ();
    callStepJs = engine->evaluate ("(function (step) {"
                                       "var args = new Array;"
                                       "for (var i = 1; i < arguments.length; i++)"
                                           "args [i - 1] = arguments [i];"
                                       "step.call (args);"
                                   "})");

    cliFiles thisCliFiles (engine);
    viewer.rootContext ()->setContextProperty ("LOCALFILES", &thisCliFiles);
    viewer.rootContext ()->setContextProperty ("cliQml", argv [1]);
    viewer.setMainQmlFile(QStringLiteral("qml/BGMRPClientQuick/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
