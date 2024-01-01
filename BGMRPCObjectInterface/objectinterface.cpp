#include "objectinterface.h"

#include <bgmrpccommon.h>

#include <QPointer>

#include "caller.h"

using namespace NS_BGMRPCObjectInterface;

ObjectInterface::ObjectInterface(QObject* parent) : QObject(parent) {
    // FINISHED 修正对象连接控制socket的名称
    /*m_ctrlSocket->connectToServer(
        BGMRPCObjectCtrlSocket);
    if (m_ctrlSocket->waitForConnected(-1))
        qInfo().noquote()
            << QString("Object(%1),connect_server,Connect to BGMRPC ok")
                   .arg(m_name);
    else
        qWarning() << QString(
                          "Object(%1),connect_server,Connect to BGMRPC Fail")
                          .arg(m_name);

    QObject::connect(
        m_ctrlSocket, &QLocalSocket::disconnected, [&]() {
            qInfo().noquote()
                << QString("Object(%1),disconnected,disconnected").arg(m_name);
            emit objectDisconnected();
            m_dataServer->close();
        });*/
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

/*QLocalSocket*
ObjectInterface::plugIntoBGMRPC(const QByteArray& group, const QByteArray& app,
                                const QByteArray& name) {
    QLocalSocket* plug = new QLocalSocket;
    plug->connectToServer(BGMRPCObjectSocket);
    if (plug->waitForConnected(-1)) {
        QByteArray data(1, (quint8)NS_BGMRPC::CTRL_REGISTER);
        data.append(refObjName(group, app, name));
        plug->write(data);

        qInfo().noquote() << QString("Object Control connect to BGMRPC OK");

        return plug;
    } else {
        delete plug;

        qWarning().noquote()
            << QString("Object Control connect to BGMRPC FAIL");

        return nullptr;
    }
}*/

bool
ObjectInterface::setup(const QByteArray& appName, const QByteArray& name,
                       const QByteArray& grp, int argc, char** argv,
                       bool noAppPrefix) {
    QByteArray objID = genObjectName(grp, appName, name, noAppPrefix);

    m_objectPlug = new QLocalSocket;
    m_objectPlug->connectToServer(BGMRPCObjectSocket);
    if (m_objectPlug->waitForConnected(-1)) {
        QByteArray checkObj(1, (quint8)NS_BGMRPC::CTRL_CHECKOBJECT);
        checkObj.append(objID);
        m_objectPlug->write(checkObj);
        if (!m_objectPlug->waitForBytesWritten() ||
            !m_objectPlug->waitForReadyRead()) {
            qWarning().noquote()
                << QString("CheckObject(%1), Can't request").arg(objID);
            return false;
        } else if ((quint8)(m_objectPlug->readAll()[0])) {
            qWarning().noquote() << QString(
                                        "An object named \"%1\" already "
                                        "exists on the BGMRPC")
                                        .arg(objID);
            return false;
        }

        m_name = name;
        m_ID = objID;
        m_grp = noAppPrefix ? "" : grp;
        m_appName = appName;

        QString rootPath =
            getSettings(*m_objectPlug, NS_BGMRPC::CNF_PATH_ROOT);
        QString dataPath = rootPath + "/data/" +
                              (m_grp.isEmpty() ? "default" : m_grp) + '/' +
                              m_appName.section("::", 0, 0);
        setAppPath(rootPath + "/apps/" + m_appName.section("::", -1, -1));
        setDataPath(dataPath);

        if (!initial(argc, argv)) return false;

        QString dataServerName = BGMRPCObjPrefix + m_ID;
        m_dataServer = new QLocalServer(this);
        if (m_dataServer->listen(dataServerName)) {
            QByteArray data(1, (quint8)NS_BGMRPC::CTRL_REGISTER);
            data.append(m_ID.toLatin1());
            m_objectPlug->write(data);
            if (!m_objectPlug->waitForBytesWritten() ||
                !m_objectPlug->waitForReadyRead()) {
                qWarning().noquote()
                    << QString("Register Object(%1), Can't request").arg(m_ID);
            } else if (!(quint8)m_objectPlug->readAll()[0]) {
                qWarning().noquote() << QString(
                                            "An object named \"%1\" already "
                                            "exists on the BGMRPC")
                                            .arg(m_ID);

                return false;
            } else
                qInfo().noquote()
                    << QString("Object(%1) plug to BGMRPC OK").arg(m_ID);

            QObject::connect(m_dataServer, &QLocalServer::newConnection, this,
                             &ObjectInterface::newCaller);

            QObject::connect(this, &ObjectInterface::thread_signal_call, this,
                             &ObjectInterface::on_thread_call);

            QObject::connect(m_objectPlug, &QLocalSocket::disconnected, this,
                             [&]() {
                                 detachObject();

                                 QCoreApplication::quit();
                             });

            qInfo().noquote()
                << QString("Object(%1),registerObject,ready").arg(m_ID);

            return true;
        } else {
            qWarning().noquote()
                << QString("Object(%1)(%2),registerObject,Not ready")
                       .arg(QString(m_ID), dataServerName);
            // detachObject();

            return false;
        }
    } else {
        delete m_objectPlug;

        qWarning().noquote()
            << QString("Object(%1) connect to BGMRPC FAIL").arg(objID);

        return false;
    }
}

/*bool
ObjectInterface::registerObject(const QByteArray& appName,
                                const QByteArray& name, const QByteArray& grp) {
    if (!m_ctrlSocket->isOpen()) {
        //        qWarning() << "Register fail";
        return false;
    } else {
        QByteArray ctrl(1, (quint8)NS_BGMRPC::CTRL_REGISTER);
        //        ctrl[0] = (quint8)NS_BGMRPC::CTRL_REGISTER;
        QByteArray objName = grp.isEmpty() ? name : grp + "::" + name;
        ctrl.append(objName);
        m_ctrlSocket->write(ctrl);

        QString socketServerName =  BGMRPCObjPrefix + objName;
        if (m_dataServer->listen(socketServerName)) {
            m_name = name;
            m_grp = grp;
            m_appName = appName;
            qInfo().noquote()
                << QString("Object(%1),registerObject,ready").arg(m_name);
            return true;
        } else {
            qWarning().noquote()
                << QString("Object(%1)(%2),registerObject,Not ready")
                       .arg(QString(objName))
                       .arg(socketServerName);
            return false;
        }
    }
}*/

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

/*!
 * \brief 将调用者(客户端)与对象关联
 * \param caller 调用者
 * \note 将客户端关联对象，其目的在于：
 * + 对象在广播信号时只向其所关联的客户端发送信号，而非所有客户端
 * + 对象可在关联客户端中查找
 */
void
ObjectInterface::addRelatedCaller(QPointer<Caller> caller) {
    if (caller.isNull() || caller->exited()) return;

    m_relatedCaller[caller->m_ID] = caller;
    QObject::connect(caller.data(), &Caller::clientExited, [=]() {
        if (m_relatedCaller.contains(caller->m_ID)) {
            relatedCallerExited(caller);
            m_relatedCaller.remove(caller->m_ID);
            QObject::disconnect(caller.data(), &Caller::clientExited, nullptr,
                                nullptr);
        }
    });
}

bool
ObjectInterface::removeRelatedCaller(QPointer<Caller> caller) {
    if (caller.isNull() || caller->exited()) return false;

    if (m_relatedCaller.contains(caller->m_ID)) {
        relatedCallerExited(caller);
        m_relatedCaller.remove(caller->m_ID);
        QObject::disconnect(caller.data(), &Caller::clientExited, nullptr,
                            nullptr);
        return true;
    } else
        return false;
}

QPointer<Caller>
ObjectInterface::findRelatedCaller(
    std::function<bool(QPointer<Caller>)> callback) {
    foreach (QPointer<Caller> caller, m_relatedCaller) {
        if (callback(caller)) return caller;
    }

    return QPointer<Caller>();
}

void
ObjectInterface::emitSignal(const QString& signal, const QVariant& args) {
    foreach (QPointer<Caller> caller, m_relatedCaller)
        caller->emitSignal(signal, args);
}

/*!
 * \brief 远端对象之间的调用
 * \param caller 调用者
 * \param object 调用对象
 * \param method 调用对象的方法
 * \param args 参数
 * \return 返回对象方法的处理结果
 * \note 调用者为客户端而非远端对象
 */
QVariant
ObjectInterface::call(QPointer<Caller> caller, const QString& object,
                      const QString& method, const QVariantList& args) {
    QEventLoop CL_loop;
    QVariant retData;

    emit thread_signal_call(true, caller.isNull() ? -1 : caller->m_ID, object,
                            method, args);
    QObject::connect(this, &ObjectInterface::thread_signal_return,
                     [&](const QVariant& data) {
                         retData = data;
                         CL_loop.exit();
                     });

    CL_loop.exec();
    QObject::disconnect(this, &ObjectInterface::thread_signal_return, nullptr,
                        nullptr);

    return retData;
}

/*!
 * \overload
 * \note
 * 忽略调用者参数，如果被调用的远端对象无需操作原始调用者客户端(例如私有数据)，
 * 则调用此成员函数
 */
QVariant
ObjectInterface::call(const QString& object, const QString& method,
                      const QVariantList& args) {
    return call(nullptr, object, method, args);
}

void
ObjectInterface::callNonblock(const QString& object, const QString& method,
                              const QVariantList& args) {
    emit thread_signal_call(false, -2, object, method, args);
}

void
ObjectInterface::setPrivateData(QPointer<Caller> caller, const QString& name,
                                const QVariant& data) {
    quint64 cliID = caller->m_ID;
    if (!m_privateDatas.contains(cliID)) m_privateDatas[cliID] = QVariantMap();

    if (data.isNull()) {
        m_privateDatas[cliID].remove(name);
    } else
        m_privateDatas[cliID][name] = data;
}

QVariant
ObjectInterface::privateData(QPointer<Caller> caller,
                             const QString& name) const {
    quint64 cliID = caller->m_ID;
    if (!m_privateDatas.contains(cliID)) return QVariant();

    return m_privateDatas[cliID][name];
}

void
ObjectInterface::detachObject() {
    qInfo().noquote()
        << QString("Object(%1),disconnected,disconnected").arg(m_ID);
    m_dataServer->close();
    m_dataServer->deleteLater();
    // m_ctrlSocket->disconnectFromServer();
    // m_ctrlSocket->deleteLater();
    m_objectPlug->disconnectFromServer();
    m_objectPlug->deleteLater();
    deleteLater();
}

void
ObjectInterface::newCaller() {
    QLocalSocket* cliDataSocket = m_dataServer->nextPendingConnection();
    QPointer<Caller> caller = new Caller(this, cliDataSocket);

    QObject::connect(cliDataSocket, &QLocalSocket::readyRead, [=]() {
        auto f = [=](const QByteArray& dataFrame) {
            QJsonDocument callJsonDoc = QJsonDocument::fromJson(dataFrame);

            QString methodName = callJsonDoc["method"].toString();
            QVariantList args = callJsonDoc["args"].toVariant().toList();
            QString mID = callJsonDoc["mID"].toString("#");

            if (caller->m_callType == NS_BGMRPC::CALL_UNDEFINED) {
                caller->m_callType =
                    (NS_BGMRPC::Call)callJsonDoc["callType"].toInt();

                caller->m_ID = callJsonDoc["callerID"].toInt();

                if (caller->m_callType == NS_BGMRPC::CALL_INTERNAL ||
                    caller->m_callType == NS_BGMRPC::CALL_INTERNAL_NOBLOCK) {
                    caller->m_callerApp = callJsonDoc["callerApp"].toString();
                    caller->m_callerObject =
                        callJsonDoc["callerObject"].toString();
                    caller->m_callerObjectID =
                        callJsonDoc["callerObjectID"].toString();
                    caller->m_callerGrp = callJsonDoc["callerGrp"].toString();

                    if (caller->m_callType == NS_BGMRPC::CALL_INTERNAL_NOBLOCK)
                        caller->unsetDataSocket();
                }
            }

            /*if (caller->m_localCall) {
                caller->m_callerObject = callJsonDoc["callerObject"].toString();
                caller->m_callerGrp = callJsonDoc["callerGrp"].toString();
            }*/

            if (!m_methods[methodName])
                caller->returnError(mID, NS_BGMRPC::ERR_NOMETHOD,
                                    m_ID + '.' + methodName);
            else {
                caller->m_calleeMethod = methodName;
                exec(mID, caller, methodName, args);
            }
        };

        splitLocalSocketFragment(cliDataSocket, f);
    });

    QObject::connect(caller.data(), &Caller::clientExited, [=]() {
        // if (!caller->m_localCall) {
        if (caller->m_callType == NS_BGMRPC::CALL_REMOTE) {
            emit callerExisted(caller);
            m_privateDatas.remove(caller->m_ID);
        }
    });
}

void
ObjectInterface::on_thread_call(bool block, qint64 callerID,
                                const QString& object, const QString& method,
                                const QVariantList& args) {
    QByteArray checkObj(1, (quint8)NS_BGMRPC::CTRL_CHECKOBJECT);
    checkObj.append(object.toLatin1());
    m_objectPlug->write(checkObj);
    if (!m_objectPlug->waitForBytesWritten()) {
        qWarning() << QString(
                          "Object(%1),localCall-checkObject,Can't request "
                          "check object(%2)")
                          .arg(m_ID)
                          .arg(object);
        emit thread_signal_return(QVariant());
        return;
    }
    if (!m_objectPlug->waitForReadyRead() ||
        !(quint8)(m_objectPlug->readAll()[0])) {
        qWarning() << QString(
                          "Object(%1),localCall-checkObject,"
                          "(Call local method)The requested "
                          "object(%2) does not exist")
                          .arg(m_ID)
                          .arg(object);
        emit thread_signal_return(QVariant());
        return;
    }

    QVariantMap callVariant;
    callVariant["object"] = object;
    callVariant["method"] = method;
    callVariant["args"] = args;
    callVariant["callerApp"] = m_appName;
    callVariant["callerObject"] = m_name;
    callVariant["callerObjectID"] = m_ID;
    callVariant["callerGrp"] = m_grp;
    callVariant["callerID"] = callerID;
    callVariant["callType"] =
        block ? NS_BGMRPC::CALL_INTERNAL : NS_BGMRPC::CALL_INTERNAL_NOBLOCK;

    QLocalSocket* localCallSocket = new QLocalSocket;
    localCallSocket->connectToServer(BGMRPCObjPrefix + object);
    if (!localCallSocket->waitForConnected()) {
        emit thread_signal_return(QVariant());
        return;
    }

    QByteArray callData;
    /*if (block == false) {
        callData.resize(1);
        callData[0] = (quint8)NS_BGMRPC::DATA_NONBLOCK_LOCALCALL;
    } else*/
    /* if (callerID >= 0)*/ /*{
callData.resize(1);
callData[0] = (quint8)NS_BGMRPC::DATA_LOCALCALL_CLIENTID;
callData.append(
   int2bytes<quint64>(callerID >= 0 ? callerID : 0xffffffff));
}*/

    QByteArray callJsonData = QJsonDocument::fromVariant(callVariant).toJson();
    callData += int2bytes<quint64>(callJsonData.length());
    callData += callJsonData;
    localCallSocket->write(callData);
    if (!localCallSocket->waitForBytesWritten()) {
        localCallSocket->deleteLater();
        emit thread_signal_return(QVariant());
        return;
    }

    if (block) {
        //        bool capturedReturn = false;
        //        QVariant retVariant;
        /*while (localCallSocket->waitForReadyRead()) {
            splitData(
                localCallSocket->readAll(), [&](const QByteArray& baRetData) {
                    QJsonDocument jsonDoc = QJsonDocument::fromJson(baRetData);
                    if (jsonDoc["type"].toString() == "return") {
                        capturedReturn = true;
                        retVariant = jsonDoc["values"].toVariant();
                    }
                });
            if (capturedReturn) break;
        }*/
        auto f = [=](const QByteArray& baRetData) -> QVariant {
            QJsonDocument jsonDoc = QJsonDocument::fromJson(baRetData);
            if (jsonDoc["type"].toString() == "return")
                return jsonDoc["values"].toVariant();
            else
                return QVariant();
        };
        //        int lenLen = sizeof(quint64);
        //        QVariant retVariant;
        while (localCallSocket->waitForReadyRead()) {
            if (splitLocalSocketFragment(
                    localCallSocket, [&](const QByteArray& dataFrame) {
                        QVariant retVariant = f(dataFrame);
                        if (retVariant.isValid()) {
                            emit thread_signal_return(retVariant);
                            localCallSocket->deleteLater();
                        }
                    }))
                break;
            /*if (localCallSocket->property("fragment").isValid()) {
                quint64 len =
                    localCallSocket->property("fragmentLen").toULongLong();
                QByteArray readData = localCallSocket->read(len);
                quint64 readedLen = readData.length();

                readData = localCallSocket->property("fragment").toByteArray() +
                           readData;

                if (readedLen < len) {
                    localCallSocket->setProperty("fragment", readData);
                    localCallSocket->setProperty("fragmentLen",
                                                 len - readedLen);

                    continue;
                } else {
                    retVariant = f(readData);
                    localCallSocket->setProperty("fragment", QVariant());
                    localCallSocket->setProperty("fragmentLen", QVariant());
                    if (retVariant.isValid()) {
                        localCallSocket->deleteLater();
                        thread_signal_return(retVariant);
                        return;
                    }
                }
            }

            while (localCallSocket->bytesAvailable() > 0) {
                quint64 len = bytes2int<quint64>(localCallSocket->read(lenLen));
                QByteArray readData = localCallSocket->read(len);
                quint64 readedLen = readData.length();
                if (readedLen < len) {
                    localCallSocket->setProperty("fragment", readData);
                    localCallSocket->setProperty("fragmentLen",
                                                 len - readedLen);
                } else {
                    retVariant = f(readData);
                    if (retVariant.isValid()) {
                        localCallSocket->deleteLater();
                        thread_signal_return(retVariant);
                        return;
                    }
                }
            }*/
        }
    } else
        localCallSocket->deleteLater();
}

bool
ObjectInterface::initial(int /*argc*/, char** /*argv*/) {
    registerMethods();
    return true;
}

bool
ObjectInterface::verification(QPointer<Caller> /*caller*/,
                              const QString& /*method*/,
                              const QVariantList& /*args*/) {
    return true;
}

void
ObjectInterface::exec(const QString& mID, QPointer<Caller> caller,
                      const QString& methodName, const QVariantList& args) {
    QThread* callThread = QThread::create([=]() {
        if (!caller.isNull() && verification(caller, methodName, args)) {
            qInfo().noquote() << QString("Caller(%1),call,Calling %2.%3(%4)")
                                     .arg(caller->m_ID)
                                     .arg(m_ID)
                                     .arg(methodName)
                                     .arg(mID);
            QVariant ret = m_methods[methodName](
                caller, args);  // m_methods[methodName](this, caller, args);

            if (!caller.isNull()) {
                qInfo().noquote()
                    << QString("Obeject(%1),called,%2(%3) has been Called")
                           .arg(m_ID)
                           .arg(methodName)
                           .arg(mID);
                emit caller->returnData(mID, ret);
            }
        } else if (!caller.isNull()) {
            emit caller->emitSignal("ERROR_ACCESS", { methodName });
            emit caller->returnError(mID, NS_BGMRPC::ERR_ACCESS,
                                     m_ID + '.' + methodName);
            emit caller->returnData(mID, QVariant());
            qWarning().noquote()
                << QString("Object(%1),access,Not allow(%2) call %1.%3")
                       .arg(m_ID)
                       .arg(caller->m_ID)
                       .arg(mID);
        } else
            qWarning().noquote()
                << QString("(%1.%2),call,Caller may have disconnected")
                       .arg(m_ID)
                       .arg(methodName);
    });

    QObject::connect(callThread, &QThread::finished, callThread,
                     &QThread::deleteLater);

    callThread->start();
}
