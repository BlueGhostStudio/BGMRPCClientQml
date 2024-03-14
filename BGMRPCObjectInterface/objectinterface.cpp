#include "objectinterface.h"

#include <caller.h>

#include "objctrlcmdcontroller.h"

using namespace NS_BGMRPCObjectInterface;

t_method::t_method(const QString& desc, bool async)
    : m_desc(desc), m_isAsync(async) {}

ObjectInterface::ObjectInterface(QObject* parent) : QObject{ parent } {}

ObjectInterface::~ObjectInterface() {
    detachObject();
    logFile.close();
}

void
ObjectInterface::setAppPath(const QString& path) {
    m_appPath = path;
}

QString
ObjectInterface::appPath() const {
    return m_appPath;
}

void
ObjectInterface::setDataPath(const QString& path) {
    m_dataPath = path;
}

QString
ObjectInterface::dataPath() const {
    return m_dataPath;
}

QString
ObjectInterface::objectName() const {
    return m_name;
}

QString
ObjectInterface::objectID() const {
    return m_ID;
}

QString
ObjectInterface::group() const {
    return m_grp;
}

QString
ObjectInterface::appName() const {
    return m_appName;
}

QVariant
ObjectInterface::interface(QPointer<Caller>, bool readable) {
    if (readable) {
        QString methodsInfo;

        QHash<QString, t_method>::const_iterator it;
        for (it = m_methods.constBegin(); it != m_methods.constEnd(); ++it) {
            if (!methodsInfo.isEmpty()) methodsInfo.append("\n\n");

            t_method theMethod = it.value();
            methodsInfo.append(theMethod.m_decl);
            if (!theMethod.m_desc.isEmpty())
                methodsInfo.append("\n").append(theMethod.m_desc);
        }

        return methodsInfo;
    } else {
        QVariantList methodsInfo;

        QHash<QString, t_method>::const_iterator it;
        for (it = m_methods.constBegin(); it != m_methods.constEnd(); ++it) {
            t_method theMethod = it.value();

            methodsInfo.append(
                QVariantMap{ { "Declaration", theMethod.m_decl },
                             { "Description", theMethod.m_desc } });
        }

        return methodsInfo;
    }
}

bool
ObjectInterface::setup(const QByteArray& appName, const QByteArray& name,
                       const QByteArray& grp, int argc, char** argv,
                       bool noAppPrefix) {
    m_ID = genObjectName(grp, appName, name, noAppPrefix);
    m_name = name;
    m_grp = noAppPrefix ? "" : grp;
    m_appName = appName;
    qInfo().noquote() << "    group: " << m_grp;
    qInfo().noquote() << "      app: " << m_appName;
    qInfo().noquote() << "     name: " << m_name;
    qInfo().noquote() << "object ID: " << m_ID;

    QString dataServerName = BGMRPCObjPrefix + m_ID;

    try {
        m_objectConnecter = new QLocalSocket;
        m_objectConnecter->connectToServer(BGMRPCObjectSocket);
        if (!m_objectConnecter->waitForConnected(-1)) {
            throw std::runtime_error(
                QString("The object (%1) cannot be associated with the server.")
                    .arg(QString(m_ID))
                    .toStdString());
        }

        /*
         *  check object exists
         */
        if (checkObject(m_objectConnecter, m_ID)) {
            qWarning().noquote() << QString(
                                        "A server with the name %1 already "
                                        "exists for the object.")
                                        .arg(m_ID);
            return false;
        }

        /*
         *  start object server
         */

        QString rootPath =
            getSettings(*m_objectConnecter, NS_BGMRPC::CNF_PATH_ROOT);
        setAppPath(rootPath + "/apps/" + m_appName.section("::", -1, -1));
        setDataPath(rootPath + "/data/" +
                    (m_grp.isEmpty() ? "default" : m_grp) + '/' +
                    m_appName.section("::", 0, 0));

        if (!initial(argc, argv))  // initial
            return false;

        m_dataServer = new QLocalServer(this);
        if (m_dataServer->listen(dataServerName)) {
            QObject::connect(m_dataServer, &QLocalServer::newConnection, this,
                             &ObjectInterface::newCaller);
            QObject::connect(m_objectConnecter, &QLocalSocket::disconnected,
                             this, [&]() {
                                 detachObject();
                                 QCoreApplication::quit();
                             });
        } else {
            qWarning().noquote() << QString(
                                        "Data listening for the object (%1) "
                                        "failed to be established.")
                                        .arg(QString(m_ID), dataServerName);
            return false;
        }

        /*
         *  register object
         */
        if (!ctrlCommand<bool>(
                m_objectConnecter, (quint8)NS_BGMRPC::CTRL_REGISTER,
                m_ID.toLatin1(), [=](const QByteArray& data) -> bool {
                    return (quint8)data[0];
                })) {
            qWarning().noquote()
                << QString("Failed to register the object (%1).").arg(m_ID);
            return false;
        } else {
            qInfo().noquote()
                << QString("The object (%1) has been successfully registered.")
                       .arg(m_ID);

            return true;
        }
    } catch (std::exception& e) {
        qWarning().noquote() << e.what();
        return false;
    }
}

QByteArray
ObjectInterface::objCtrlCmd(quint8 cmd, const QByteArray& arg) {
    ObjCtrlCmdController objCtrl(this);
    return objCtrl.ctrlCmd(cmd, arg);
}

QVariant
ObjectInterface::call(bool block, QPointer<Caller> caller, const QString& obj,
                      const QString& method, const QVariantList& args) {
    QMutexLocker locker(&m_objMutex);

    QLocalSocket* localCallConnecter = new QLocalSocket;

    try {
        QVariant vReturn;

        localCallConnecter->connectToServer(BGMRPCObjPrefix + obj);
        if (!localCallConnecter->waitForConnected())
            throw std::runtime_error("Can't connect to obj");

        qint64 callerID = caller.isNull() ? -1 : caller->m_ID;
        QVariantMap vCallData{ { "object", obj },
                               { "method", method },
                               { "args", args },
                               { "callerApp", m_appName },
                               { "callerObject", m_name },
                               { "callerObjectID", m_ID },
                               { "callerGrp", m_grp },
                               { "callerID", callerID },
                               { "callType",
                                 block ? NS_BGMRPC::CALL_INTERNAL
                                       : NS_BGMRPC::CALL_INTERNAL_NOBLOCK } };

        QByteArray baCallData;
        QByteArray baCallJsonData =
            QJsonDocument::fromVariant(vCallData).toJson();
        baCallData += int2bytes<quint64>(baCallJsonData.length());
        baCallData += baCallJsonData;
        localCallConnecter->write(baCallData);
        if (!localCallConnecter->waitForBytesWritten())
            throw std::runtime_error("fall send call data");

        if (block) {
            while (localCallConnecter->waitForReadyRead()) {
                if (splitLocalSocketFragment(
                        localCallConnecter, [&](const QByteArray& dataFrame) {
                            QJsonDocument jsonDoc =
                                QJsonDocument::fromJson(dataFrame);
                            if (jsonDoc["type"] == "return")
                                vReturn = jsonDoc["values"].toVariant();
                        }))
                    break;
            }
        }

        localCallConnecter->deleteLater();

        return vReturn;
    } catch (std::exception& e) {
        qWarning() << "in ObjectInterface::call -- " << e.what();
        localCallConnecter->deleteLater();
        return QVariant();
    }
}

QVariant
ObjectInterface::call(QPointer<Caller> caller, const QString& obj,
                      const QString& method, const QVariantList& args) {
    return call(true, caller, obj, method, args);
}

QVariant
ObjectInterface::call(const QString& obj, const QString& method,
                      const QVariantList& args) {
    return call(true, nullptr, obj, method, args);
}

void
ObjectInterface::callNonblock(const QString& obj, const QString& method,
                              const QVariantList& args) {
    call(false, nullptr, obj, method, args);
}

void
ObjectInterface::addRelatedCaller(QPointer<Caller> caller) {
    if (caller.isNull() || caller->exited()) return;

    QMutexLocker locker(&m_objMutex);
    m_relatedCaller[caller->m_ID] = caller;
}

bool
ObjectInterface::removeRelatedCaller(QPointer<Caller> caller) {
    if (caller.isNull() || caller->exited()) return false;

    if (m_relatedCaller.contains(caller->m_ID)) {
        QMutexLocker locker(&m_objMutex);
        emit relatedCallerExited(caller);
        m_relatedCaller.remove(caller->m_ID);

        return true;
    } else
        return false;
}

bool
ObjectInterface::containsRelatedCall(QPointer<Caller> caller) {
    QMutexLocker locker(&m_objMutex);
    return caller && m_relatedCaller.contains(caller->m_ID);
}

QPointer<Caller>
ObjectInterface::findRelatedCaller(
    std::function<bool(QPointer<Caller>)> callback) {
    QMutexLocker locker(&m_objMutex);

    foreach (QPointer<Caller> caller, m_relatedCaller) {
        if (callback(caller)) return caller;
    }

    return QPointer<Caller>();
}

void
ObjectInterface::emitSignal(const QString& signal, const QVariant& args) {
    QMutexLocker locker(&m_objMutex);

    foreach (QPointer<Caller> caller, m_relatedCaller) {
        if (!caller.isNull() && !caller->exited())
            emit caller->emitSignal(signal, args);
    }
}

void
ObjectInterface::asyncReturn(QPointer<Caller> caller,
                             const QVariantMap& callInfo,
                             const QVariant& retData) {
    if (!caller.isNull()) {
        emit caller->returnData(callInfo["mID"].toString(), retData,
                                callInfo["method"].toString());
    }
}

void
ObjectInterface::setPrivateData(QPointer<Caller> caller, const QString& name,
                                const QVariant& data) {
    if (caller.isNull() || caller->exited()) return;

    QMutexLocker locker(&m_objMutex);
    quint64 cliID = caller->m_ID;

    if (data.isNull()) {
        if (m_privateDatas.contains(cliID)) m_privateDatas[cliID].remove(name);
    } else {
        if (!m_privateDatas.contains(cliID))
            m_privateDatas[cliID] = QVariantMap();

        m_privateDatas[cliID][name] = data;
    }
}

QVariant
ObjectInterface::privateData(QPointer<Caller> caller, const QString& name) {
    if (caller.isNull() || caller->exited()) return QVariant();

    QMutexLocker locker(&m_objMutex);
    quint64 cliID = caller->m_ID;

    if (m_privateDatas.contains(cliID))
        return m_privateDatas[cliID][name];
    else
        return QVariant();
}

void
ObjectInterface::detachObject() {
    m_dataServer->close();
    m_dataServer->deleteLater();
    m_objectConnecter->disconnectFromServer();
    m_objectConnecter->deleteLater();
    deleteLater();

    qInfo().noquote() << QString("Object(%1),disconnected").arg(m_ID);
}

void
ObjectInterface::objCtrlCmdWork(quint8 cmd, const QByteArray& arg) {
    QMutexLocker locker(&m_objMutex);

    ctrlCommand<void>(m_objectConnecter, cmd, arg, [&](const QByteArray& data) {
        emit objCtrlCmdReady(data);
    });
}

void
ObjectInterface::newCaller() {
    QLocalSocket* cliDataSlot = m_dataServer->nextPendingConnection();
    Caller* caller = new Caller(this, cliDataSlot);
    QPointer<Caller> callerPtr(caller);

    QObject::connect(cliDataSlot, &QLocalSocket::readyRead, this, [=]() {
        auto f = [=](const QByteArray& dataFrame) {
            QJsonDocument callJsonDoc = QJsonDocument::fromJson(dataFrame);

            QString methodName = callJsonDoc["method"].toString();
            QVariantList args = callJsonDoc["args"].toVariant().toList();
            QString mID = callJsonDoc["mID"].toString("#");

            if (!m_methods.contains(methodName)) {
                qWarning().noquote() << "The \"" + m_ID +
                                            "\" object does not have the \"" +
                                            methodName + "\" method.";
                emit callerPtr->returnError(mID, NS_BGMRPC::ERR_NOMETHOD,
                                            m_ID + '.' + methodName);
            } else {
                if (callerPtr->m_callType == NS_BGMRPC::CALL_UNDEFINED) {
                    callerPtr->m_callType =
                        (NS_BGMRPC::Call)callJsonDoc["callType"].toInt();

                    callerPtr->m_ID = callJsonDoc["callerID"].toInt();

                    if (callerPtr->m_callType == NS_BGMRPC::CALL_INTERNAL ||
                        callerPtr->m_callType ==
                            NS_BGMRPC::CALL_INTERNAL_NOBLOCK) {
                        callerPtr->m_callerApp =
                            callJsonDoc["callerApp"].toString();
                        callerPtr->m_callerObject =
                            callJsonDoc["callerObject"].toString();
                        callerPtr->m_callerObjectID =
                            callJsonDoc["callerObjectID"].toString();
                        callerPtr->m_callerGrp =
                            callJsonDoc["callerGrp"].toString();

                        if (callerPtr->m_callType ==
                            NS_BGMRPC::CALL_INTERNAL_NOBLOCK)
                            callerPtr->unsetDataSocket();
                    }
                }

                exec(mID, callerPtr, methodName, args);
            }
        };

        splitLocalSocketFragment(cliDataSlot, f);
    });

    QObject::connect(
        caller, &Caller::clientExited, this,
        [=](quint64 id) {
            emit callerExited(callerPtr);

            m_privateDatas.remove(id);
            if (m_relatedCaller.contains(id)) {
                emit relatedCallerExited(callerPtr);
                m_relatedCaller.remove(id);
            }
        },
        Qt::SingleShotConnection);
}

bool
ObjectInterface::initial(int, char**) {
    RM("interface", { "Method list of the interface" },
       &ObjectInterface::interface, ARG<bool>("readable", true));
    registerMethods();

    return true;
}

bool
ObjectInterface::verification(QPointer<Caller>, const QString&,
                              const QVariantList&) {
    return true;
}
void
ObjectInterface::exec(const QString& mID, QPointer<Caller> caller,
                      const QString& method, const QVariantList& args) {
    QThread* callThread = QThread::create([=]() {
        if (!caller.isNull()) {
            caller->callIncrement();
            if (verification(caller, method, args)) {
                qInfo().noquote() << QString("Caller(%1), Calling %2.%3(%4)")
                                         .arg(caller->m_ID)
                                         .arg(m_ID)
                                         .arg(method)
                                         .arg(mID);
                try {
                    const t_method& theMethod = m_methods[method];

                    QVariantList __args__(std::move(args));
                    if (theMethod.m_isAsync)
                        __args__.prepend(QVariantMap{ { "mID", mID },
                                                      { "method", method } });

                    QVariant ret = theMethod.m_methodPtr(caller, __args__);
                    if (!caller.isNull()) {
                        qInfo().noquote()
                            << QString(
                                   "Object(%1), called, %2(%3) has been Called")
                                   .arg(m_ID)
                                   .arg(method)
                                   .arg(mID);

                        if (!theMethod.m_isAsync)
                            emit caller->returnData(mID, ret, method);
                    }
                } catch (std::exception& e) {
                    /*emit caller->emitSignal("ERROR_INVALID_ARGUMENT",
                                            { method });*/
                    emit caller->returnError(
                        mID, NS_BGMRPC::ERR_INVALID_ARGUMENT,
                        QString("%1\n%2").arg(e.what()).arg(
                            m_methods[method].m_decl));
                };
            } else {
                // emit caller->emitSignal("ERROR_ACCESS", { method });
                emit caller->returnError(mID, NS_BGMRPC::ERR_ACCESS,
                                         m_ID + '.' + method);
                // emit caller->returnData(mID, QVariant(), method);
                qWarning().noquote()
                    << QString("Object(%1), access, Not allow(%2) call %1.%3")
                           .arg(m_ID)
                           .arg(caller->m_ID)
                           .arg(mID);
            }
        } else
            qWarning().noquote()
                << QString("(%1.%2), call, Caller may have disconnected")
                       .arg(m_ID)
                       .arg(method);
    });

    QObject::connect(callThread, &QThread::finished, callThread,
                     &QThread::deleteLater);

    callThread->start();
}
