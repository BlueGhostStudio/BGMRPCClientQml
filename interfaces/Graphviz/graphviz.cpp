#include "graphviz.h"

#include <QProcess>

Graphviz::Graphviz(QObject* parent) : ObjectInterface(parent) {}

QVariant
Graphviz::render(QPointer<Caller> caller, const QVariantList& args) {
    QMutex mutex;
    QMutexLocker locker(&mutex);

    if (!args[0].canConvert<QByteArray>())
        return QVariantMap(
            { { "ok", false }, { "error", "Parameter type error" } });

    QByteArray dotInput(args[0].toByteArray());

    QProcess dotProc(this);
    dotProc.start("dot", { "-T", "svg" });

    if (!dotProc.waitForStarted(1000))
        return QVariantMap({ { "ok", false },
                             { "error",
                               "The process cannot start, possibly because "
                               "Graphviz is not installed." } });

    dotProc.write(dotInput);
    dotProc.closeWriteChannel();

    if (!dotProc.waitForFinished(1000)) {
        dotProc.terminate();

        return QVariantMap(
            { { "ok", false },
              { "error",
                "The process did not finish normally; it is still running. "
                "Forced termination initiated." } });
    }

    if (dotProc.exitStatus() == QProcess::CrashExit)
        return QVariantMap({ { "ok", false },
                             { "error", "Dot (Graphviz) process crashed." } });

    if (dotProc.exitCode() != 0)
        return QVariantMap(
            { { "ok", false },
              { "error",
                dotProc.readAllStandardError() } });  // clang-format on

    return QVariantMap(
        { { "ok", true }, { "svg", dotProc.readAllStandardOutput() } });
}

void
Graphviz::registerMethods() {
    REG_METHOD("render", &Graphviz::render);
    qDebug() << "methods" << m_methods.keys();
}

ObjectInterface*
create(int, char**) {
    return new Graphviz;
}
