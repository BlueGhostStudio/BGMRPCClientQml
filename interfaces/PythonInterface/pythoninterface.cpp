#include "pythoninterface.h"

#include <getopt.h>

#include <QDir>
#include <QFileInfo>
#include <QMutexLocker>

#include "pyobj.h"

using namespace NS_BGMRPCObjectInterface;

QString __objName__;

PythonInterface::PythonInterface(QObject* parent)
    : ObjectInterface(parent), m_hasPyRuning(false) {
    PythonQt::init(PythonQt::RedirectStdOut);
    // PythonQt::init();
    m_pyMainContent = PythonQt::self()->getMainModule();
    PythonQt::self()->registerQObjectClassNames(
        {"NS_BGMRPCObjectInterface::PyCaller"});
    m_pyMainContent.addObject("OBJ", new PyObj(this));

    QObject::connect(
        PythonQt::self(), &PythonQt::pythonStdOut, [=](const QString& out) {
            qInfo().noquote()
                << QString("PythonObjectInterface(%1),%2").arg(m_name).arg(out);
        });
    QObject::connect(
        PythonQt::self(), &PythonQt::pythonStdErr, [=](const QString& out) {
            qWarning().noquote()
                << QString("PythonObjectInterface(%1),%2").arg(m_name).arg(out);
        });
}

void PythonInterface::initial(const QString& appPath, const QString& dataPath,
                              int argc, char** argv) {
    ObjectInterface::initial(appPath, dataPath, argc, argv);

    __objName__ = argv[2];
    int opt = 0;
    optind = 0;

    while ((opt = getopt(argc, argv, "s:S:")) != -1) {
        switch (opt) {
        case 's':
            QDir::setCurrent(QDir::currentPath() + '/' + optarg);
            loadPyFile("main.py");
            break;
        case 'S':
            loadPyFile(optarg);
            break;
        }
    }

    if (m_pyMainContent.getVariable("constructor").isValid()) {
        qInfo().noquote()
            << QString("PythonObjectInterface(%1),initial...").arg(__objName__);
        m_pyMainContent.call("constructor");
    }
}

/*QVariant PythonInterface::callPy(const QString& name, QPointer<Caller> cli,
                                 const QVariantList& args)
{
}*/

bool PythonInterface::loadPyFile(const QString& pyFileName) {
    qInfo().noquote() << QString(
                             "PythonObjectInterface(%1),loadPython,"
                             "Loading python file(%2)...")
                             .arg(__objName__)
                             .arg(pyFileName);
    m_pyMainContent.evalFile(pyFileName);

    bool ok = !PythonQt::self()->hadError();

    if (!ok)
        qWarning().noquote()
            << QString("PythonObjectInterface,loadPython(%1),... fail")
                   .arg(pyFileName);
    else {
        qInfo().noquote()
            << QString("PythonObjectInterface(%1),loadPython(%2),... ok")
                   .arg(__objName__)
                   .arg(pyFileName);

        m_PWD = QFileInfo(pyFileName).path() + '/';
        m_pyMainContent.addVariable("PWD", m_PWD);
        /*if (m_pyMainContent.getVariable("constructor").isValid()) {
            qInfo().noquote()
                << QString(
                       "PythonObjectInterface(%1),loadPython,Initial(%2)...")
                       .arg(__objName__)
                       .arg(pyFileName);
            m_pyMainContent.call("constructor");
        }*/
        registerMethods();
    }

    return ok;
}

void PythonInterface::mutexLock() { m_mutex.lock(); }

void PythonInterface::mutexUlock() { m_mutex.unlock(); }

void PythonInterface::registerMethods() {
    QVariantList methods = m_pyMainContent.getVariable("methods").toList();
    qInfo().noquote()
        << QString("PythonObjectInterface(%1),registMethod,Register methods...")
               .arg(__objName__);
    foreach (QVariant m, methods) {
        QString methodName = m.toString();
        qInfo().noquote()
            << QString("- PythonObjectInterface(%1),registMethod,...(%2)")
                   .arg(__objName__)
                   .arg(methodName);
        registerMethod(methodName);
    }
}

void PythonInterface::registerMethod(const QString& methodName) {
    m_methods[methodName] = std::bind(
        [&](const QString& name, ObjectInterface*, QPointer<Caller> caller,
            const QVariantList& args) -> QVariant {
            if (caller.isNull()) return QVariant();

            QMutexLocker locker(&m_mutex);
            /*if (!m_hasPyRuning) {
                qDebug() << "--| run |--" << name;
                m_hasPyRuning = true;
            } else {
                qDebug() << "--| wait |--" << name;
                m_waitForRun.wait(&m_mutex);
            }*/

            QVariantList _args_(std::move(args));
            PyCaller* thePyCaller = new PyCaller(caller);
            _args_.prepend(QVariant::fromValue(thePyCaller));
            // m_mutex.lock();
            QVariant ret = m_pyMainContent.call(name, _args_);
            // m_mutex.unlock();
            thePyCaller->deleteLater();

            /*m_hasPyRuning = false;
            qDebug() << "--| wake one |--";
            m_waitForRun.wakeOne();*/

            return ret;
        },
        methodName, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);
}

bool PythonInterface::verification(QPointer<Caller> caller,
                                   const QString& method,
                                   const QVariantList& args) {
    QMutexLocker locker(&m_mutex);
    if (m_pyMainContent.getVariable("verification").isValid()) {
        PyCaller* thePyCaller = new PyCaller(caller);

        QVariantList _args_;
        //        _args_ << 123 << 234;
        _args_.append(QVariant::fromValue(thePyCaller));
        _args_.append(method);
        _args_.append(QVariant(args));

        bool ok = m_pyMainContent.call("verification", _args_).toBool();
        //        thePyCaller->deleteLater();
        thePyCaller->deleteLater();
        return ok;
    } else
        return true;
}

/*void PythonInterface::callMethod(const QString& mID, QPointer<Caller> caller,
                                 const QString& methodName,
                                 const QVariantList& args)
{
    QMutexLocker locker(&m_mutex);
    ObjectInterface::callMethod(mID, caller, methodName, args);
}*/

ObjectInterface* create() {
    /*__objName__ = argv[2];
    int opt = 0;
    optind = 0;
    //    QString pyFile;

    PythonInterface* obj = nullptr;

    while ((opt = getopt(argc, argv, "s:S:")) != -1) {
        switch (opt) {
        case 's':
            QDir::setCurrent(QDir::currentPath() + '/' + optarg);
            obj = new PythonInterface;
            obj->loadPyFile("main.py");
            break;
        case 'S':
            obj = new PythonInterface;
            qDebug().noquote() << "-| in create |-| optarg |->" << optarg;
            obj->loadPyFile(optarg);
            break;
        }
    }

    qDebug().noquote() << "-| currentPath |->" << QDir::currentPath();

    return obj;*/
    return new PythonInterface;
}
