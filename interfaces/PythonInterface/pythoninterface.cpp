#include "pythoninterface.h"
#include "pyobj.h"
#include <QFileInfo>
#include <QMutexLocker>
#include <getopt.h>

using namespace NS_BGMRPCObjectInterface;

PythonInterface::PythonInterface(QObject* parent) : ObjectInterface(parent)
{
    PythonQt::init(PythonQt::IgnoreSiteModule);
    m_pyMainContent = PythonQt::self()->getMainModule();
    PythonQt::self()->registerQObjectClassNames(
        {"NS_BGMRPCObjectInterface::PyCaller"});
    m_pyMainContent.addObject("OBJ", new PyObj(this));
}

/*QVariant PythonInterface::callPy(const QString& name, QPointer<Caller> cli,
                                 const QVariantList& args)
{
}*/

bool PythonInterface::loadPyFile(const QString& pyFileName)
{
    qInfo() << "Loading python file" << pyFileName << "...";
    m_pyMainContent.evalFile(pyFileName);

    bool ok = !PythonQt::self()->hadError();

    if (!ok)
        qWarning() << "... fail";
    else {
        qInfo() << "... ok";

        m_PWD = QFileInfo(pyFileName).path() + '/';
        m_pyMainContent.addVariable("PWD", m_PWD);
        if (m_pyMainContent.getVariable("constructor").isValid()) {
            qInfo() << "Initial...";
            m_pyMainContent.call("constructor");
        }
        registerMethods();
    }

    return ok;
}

void PythonInterface::mutexLock()
{
    m_mutex.lock();
}

void PythonInterface::mutexUlock()
{
    m_mutex.unlock();
}

void PythonInterface::registerMethods()
{
    QVariantList methods = m_pyMainContent.getVariable("methods").toList();
    qInfo() << "Register methods...";
    foreach (QVariant m, methods) {
        QString methodName = m.toString();
        qInfo().noquote() << "..." << methodName;
        registerMethod(methodName);
    }
}

void PythonInterface::registerMethod(const QString& methodName)
{
    m_methods[methodName] = std::bind(
        [&](const QString& name, ObjectInterface*, QPointer<Caller> caller,
            const QVariantList& args) -> QVariant {
            if (caller.isNull())
                return QVariant();

            //            QMutexLocker locker(&m_mutex);
            QVariantList _args_(std::move(args));
            PyCaller* thePyCaller = new PyCaller(caller);
            thePyCaller->deleteLater();
            _args_.prepend(QVariant::fromValue(thePyCaller));
            m_mutex.lock();
            QVariant ret = m_pyMainContent.call(name, _args_);
            m_mutex.unlock();

            return ret;
        },
        methodName, std::placeholders::_1, std::placeholders::_2,
        std::placeholders::_3);
}

bool PythonInterface::verification(QPointer<Caller> caller,
                                   const QString& method,
                                   const QVariantList& args)
{
    QMutexLocker locker(&m_mutex);
    if (m_pyMainContent.getVariable("verification").isValid()) {
        PyCaller* thePyCaller = new PyCaller(caller);
        thePyCaller->deleteLater();

        QVariantList _args_;
        //        _args_ << 123 << 234;
        _args_.append(QVariant::fromValue(thePyCaller));
        _args_.append(method);
        _args_.append(QVariant(args));

        bool ok = m_pyMainContent.call("verification", _args_).toBool();
        //        thePyCaller->deleteLater();
        return ok;
    } else
        return true;
}

ObjectInterface* create(int argc, char** argv)
{
    int opt = 0;
    optind = 0;
    QString pyFile;

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
        case 'p':
            pyFile = optarg;
            break;
        }
    }

    PythonInterface* obj = new PythonInterface;
    obj->loadPyFile(pyFile);

    return obj;
}
