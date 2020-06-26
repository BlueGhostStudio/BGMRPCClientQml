#include "objectinterface.h"

#include <bgmrpccommon.h>

#include <QPointer>

#include "caller.h"

using namespace NS_BGMRPCObjectInterface;

ObjectInterface::ObjectInterface(QObject* parent)
    : QObject(parent),
      m_ctrlSocket(new QLocalSocket()),
      m_dataServer(new QLocalServer(this)) {
    m_ctrlSocket->connectToServer(/*NS_BGMRPC::*/ BGMRPCCtrlSocket);
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
            objectDisconnected();
            m_dataServer->close();
        });

    QObject::connect(m_dataServer, &QLocalServer::newConnection, this,
                     &ObjectInterface::newCaller);

    QObject::connect(this, &ObjectInterface::LC_requestCallMethod, this,
                     &ObjectInterface::on_LC_callMethod);
}

bool ObjectInterface::registerObject(const QByteArray& name) {
    if (!m_ctrlSocket->isOpen()) {
        //        qWarning() << "Register fail";
        return false;
    } else {
        QByteArray ctrl(1, (quint8)NS_BGMRPC::CTRL_REGISTER);
        //        ctrl[0] = (quint8)NS_BGMRPC::CTRL_REGISTER;
        ctrl.append(name);
        m_ctrlSocket->write(ctrl);

        QString socketServerName = /*NS_BGMRPC::*/ BGMRPCObjPrefix + name;
        if (m_dataServer->listen(socketServerName)) {
            m_name = name;
            qInfo().noquote()
                << QString("Object(%1),registerObject,ready").arg(m_name);
            return true;
        } else {
            qWarning().noquote()
                << QString("Object(%1)(%2),registerObject,Not ready")
                       .arg(QString(name))
                       .arg(socketServerName);
            return false;
        }
    }
}

QString ObjectInterface::objectName() const { return m_name; }

/*!
 * \brief 将调用者(客户端)与对象关联
 * \param caller 调用者
 * \note 将客户端关联对象，其目的在于：
 * + 对象在广播信号时只向其所关联的客户端发送信号，而非所有客户端
 * + 对象可在关联客户端中查找
 */
void ObjectInterface::addRelatedCaller(QPointer<Caller> caller) {
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

bool ObjectInterface::removeRelatedCaller(QPointer<Caller> caller) {
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

QPointer<Caller> ObjectInterface::findRelatedCaller(
    std::function<bool(QPointer<Caller>)> callback) {
    foreach (QPointer<Caller> caller, m_relatedCaller) {
        if (callback(caller)) return caller;
    }

    return QPointer<Caller>();
}

void ObjectInterface::emitSignal(const QString& signal, const QVariant& args) {
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
QVariant ObjectInterface::callLocalMethod(QPointer<Caller> caller,
                                          const QString& object,
                                          const QString& method,
                                          const QVariantList& args) {
    QEventLoop CL_loop;
    QVariant retData;

    LC_requestCallMethod(true, caller.isNull() ? -1 : caller->m_ID, object,
                         method, args);
    QObject::connect(this, &ObjectInterface::LC_return,
                     [&](const QVariant& data) {
                         retData = data;
                         CL_loop.exit();
                     });

    CL_loop.exec();
    QObject::disconnect(this, &ObjectInterface::LC_return, nullptr, nullptr);

    return retData;
}

/*!
 * \overload
 * \note
 * 忽略调用者参数，如果被调用的远端对象无需操作原始调用者客户端(例如私有数据)，
 * 则调用此成员函数
 */
QVariant ObjectInterface::callLocalMethod(const QString& object,
                                          const QString& method,
                                          const QVariantList& args) {
    return callLocalMethod(nullptr, object, method, args);
}

void ObjectInterface::callLocalMethodNonblock(const QString& object,
                                              const QString& method,
                                              const QVariantList& args) {
    LC_requestCallMethod(false, -2, object, method, args);
}

void ObjectInterface::setPrivateData(QPointer<Caller> caller,
                                     const QString& name,
                                     const QVariant& data) {
    quint64 cliID = caller->m_ID;
    if (!m_privateDatas.contains(cliID)) m_privateDatas[cliID] = QVariantMap();

    m_privateDatas[cliID][name] = data;
}

QVariant ObjectInterface::privateData(QPointer<Caller> caller,
                                      const QString& name) const {
    quint64 cliID = caller->m_ID;
    if (!m_privateDatas.contains(cliID)) return QVariant();

    return m_privateDatas[cliID][name];
}

void ObjectInterface::newCaller() {
    QLocalSocket* cliDataSocket = m_dataServer->nextPendingConnection();
    QPointer<Caller> caller = new Caller(this, cliDataSocket);

    QObject::connect(cliDataSocket, &QLocalSocket::readyRead, [=]() {
        /*QByteArray data = cliDataSocket->readAll();
        quint8 firstCh = (quint8)data[0];
        if (firstCh == NS_BGMRPC::DATA_CLIENTID ||
            firstCh == NS_BGMRPC::DATA_LOCALCALL_CLIENTID) {
            quint64 id = bytes2int<quint64>(data.mid(1, sizeof(quint64)));
            if (firstCh == NS_BGMRPC::DATA_LOCALCALL_CLIENTID)
                caller->m_localCall = true;
            caller->m_ID = id;
            data = data.mid(sizeof(quint64) + 1);

            qInfo().noquote()
                << QString("Object(%1),calling,New caller(#%2) calling")
                       .arg(m_name)
                       .arg(caller->m_ID);

            if (data.length() == 0) return;
        } else if (firstCh == NS_BGMRPC::DATA_NONBLOCK_LOCALCALL) {
            qInfo().noquote()
                << QString("QObject(%1),localCalling,Other object calling")
                       .arg(m_name);
            caller->unsetDataSocket();
            caller->m_localCall = true;
            data = data.mid(1);

            if (data.length() == 0) return;
        }
        splitData(data, [=](const QByteArray& dataFrame) {
            // qDebug().noquote() << "-[dataFrame]->" << dataFrame;
            QJsonDocument callJsonDoc = QJsonDocument::fromJson(dataFrame);

            QString methodName = callJsonDoc["method"].toString();
            QVariantList args = callJsonDoc["args"].toVariant().toList();
            QString mID = callJsonDoc["mID"].toString("#");

            if (!m_methods[methodName])
                caller->returnError(mID, NS_BGMRPC::ERR_NOMETHOD,
                                    m_name + '.' + methodName);
            else {
                caller->m_calleeMethod = methodName;
                callMethod(mID, caller, methodName, args);
            }
        });*/
        auto f = [=](const QByteArray& dataFrame) {
            QJsonDocument callJsonDoc = QJsonDocument::fromJson(dataFrame);

            QString methodName = callJsonDoc["method"].toString();
            QVariantList args = callJsonDoc["args"].toVariant().toList();
            QString mID = callJsonDoc["mID"].toString("#");

            if (!m_methods[methodName])
                caller->returnError(mID, NS_BGMRPC::ERR_NOMETHOD,
                                    m_name + '.' + methodName);
            else {
                caller->m_calleeMethod = methodName;
                callMethod(mID, caller, methodName, args);
            }
        };

        int lenLen = sizeof(quint64);

        if (!cliDataSocket->property("fragment").isValid()) {
            QByteArray readDataBuffer = cliDataSocket->readAll();
            quint8 firstCh = (quint8)readDataBuffer[0];
            if (firstCh == NS_BGMRPC::DATA_CLIENTID ||
                firstCh == NS_BGMRPC::DATA_LOCALCALL_CLIENTID) {
                quint64 id =
                    bytes2int<quint64>(readDataBuffer.mid(1, sizeof(quint64)));
                if (firstCh == NS_BGMRPC::DATA_LOCALCALL_CLIENTID)
                    caller->m_localCall = true;
                caller->m_ID = id;
                readDataBuffer = readDataBuffer.mid(sizeof(quint64) + 1);

                qInfo().noquote()
                    << QString("Object(%1),calling,New caller(#%2) calling")
                           .arg(m_name)
                           .arg(caller->m_ID);

                if (readDataBuffer.length() == 0) return;
            } else if (firstCh == NS_BGMRPC::DATA_NONBLOCK_LOCALCALL) {
                qInfo().noquote()
                    << QString("QObject(%1),localCalling,Other object calling")
                           .arg(m_name);
                caller->unsetDataSocket();
                caller->m_localCall = true;
                readDataBuffer = readDataBuffer.mid(1);

                if (readDataBuffer.length() == 0) return;
            }
            int i = 0;
            while (i < readDataBuffer.length()) {
                quint64 len = bytes2int<quint64>(readDataBuffer.mid(i, lenLen));
                i += lenLen;

                QByteArray readData = readDataBuffer.mid(i, len);
                quint64 readedLen = readData.length();
                i += readedLen;

                if (readedLen < len) {
                    cliDataSocket->setProperty("fragment", readData);
                    cliDataSocket->setProperty("fragmentLen", len - readedLen);
                } else
                    f(readData);
            }

            return;
        } else {
            quint64 len = cliDataSocket->property("fragmentLen").toULongLong();
            QByteArray readData = cliDataSocket->read(len);
            quint64 readedLen = readData.length();

            readData =
                cliDataSocket->property("fragment").toByteArray() + readData;

            if (readedLen < len) {
                cliDataSocket->setProperty("fragment", readData);
                cliDataSocket->setProperty("fragmentLen", len - readedLen);

                return;
            } else {
                f(readData);
                cliDataSocket->setProperty("fragment", QVariant());
                cliDataSocket->setProperty("fragmentLen", QVariant());
            }
        }

        while (cliDataSocket->bytesAvailable() > 0) {
            QByteArray readData = cliDataSocket->read(lenLen);

            quint64 len = bytes2int<quint64>(readData);

            readData = cliDataSocket->read(len);
            quint64 readedLen = readData.length();
            if (readedLen < len) {
                cliDataSocket->setProperty("fragment", readData);
                cliDataSocket->setProperty("fragmentLen", len - readedLen);
            } else
                f(readData);
        }
    });

    QObject::connect(caller.data(), &Caller::clientExited, [=]() {
        if (!caller->m_localCall) {
            callerExisted(caller);
            m_privateDatas.remove(caller->m_ID);
        }
    });
}

void ObjectInterface::on_LC_callMethod(bool block, qint64 callerID,
                                       const QString& object,
                                       const QString& method,
                                       const QVariantList& args) {
    QByteArray checkObj(1, (quint8)NS_BGMRPC::CTRL_CHECKOBJECT);
    checkObj.append(object);
    m_ctrlSocket->write(checkObj);
    if (!m_ctrlSocket->waitForBytesWritten()) {
        qWarning() << QString(
                          "Object(%1),localCall-checkObject,Can't request "
                          "check object(%2)")
                          .arg(m_name)
                          .arg(object);
        LC_return(QVariant());
        return;
    }
    if (!m_ctrlSocket->waitForReadyRead() ||
        !(quint8)(m_ctrlSocket->readAll()[0])) {
        qWarning() << QString(
                          "Object(%1),localCall-checkObject,"
                          "(Call local method)The requested "
                          "object(%2) does not exist")
                          .arg(m_name)
                          .arg(object);
        LC_return(QVariant());
        return;
    }

    QVariantMap callVariant;
    callVariant["object"] = object;
    callVariant["method"] = method;
    callVariant["args"] = args;

    QLocalSocket* localCallSocket = new QLocalSocket;
    localCallSocket->connectToServer(BGMRPCObjPrefix + object);
    if (!localCallSocket->waitForConnected()) {
        LC_return(QVariant());
        return;
    }

    QByteArray callData;
    if (block == false) {
        callData.resize(1);
        callData[0] = (quint8)NS_BGMRPC::DATA_NONBLOCK_LOCALCALL;
    } else if (callerID >= 0) {
        callData.resize(1);
        callData[0] = (quint8)NS_BGMRPC::DATA_LOCALCALL_CLIENTID;
        callData.append(int2bytes<quint64>(callerID));
    }

    QByteArray callJsonData = QJsonDocument::fromVariant(callVariant).toJson();
    callData += int2bytes<quint64>(callJsonData.length());
    callData += callJsonData;
    localCallSocket->write(callData);
    if (!localCallSocket->waitForBytesWritten()) {
        localCallSocket->deleteLater();
        LC_return(QVariant());
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
        int lenLen = sizeof(quint64);
        QVariant retVariant;
        while (localCallSocket->waitForReadyRead()) {
            if (localCallSocket->property("fragment").isValid()) {
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
                        LC_return(retVariant);
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
                        LC_return(retVariant);
                        return;
                    }
                }
            }
        }
    } else
        localCallSocket->deleteLater();
}

bool ObjectInterface::verification(QPointer<Caller> /*caller*/,
                                   const QString& /*method*/,
                                   const QVariantList& /*args*/) {
    return true;
}

void ObjectInterface::callMethod(const QString& mID, QPointer<Caller> caller,
                                 const QString& methodName,
                                 const QVariantList& args) {
    QThread* callThread = QThread::create([=]() {
        if (!caller.isNull() && verification(caller, methodName, args)) {
            qInfo().noquote() << QString("Caller(%1),call,Calling %2.%3(%4)")
                                     .arg(caller->m_ID)
                                     .arg(m_name)
                                     .arg(methodName)
                                     .arg(mID);
            QVariant ret = m_methods[methodName](this, caller, args);

            if (!caller.isNull()) {
                qInfo().noquote()
                    << QString("Obeject(%1),called,%2(%3) has been Called")
                           .arg(m_name)
                           .arg(methodName)
                           .arg(mID);
                caller->returnData(mID, ret);
            }
        } else if (!caller.isNull()) {
            caller->emitSignal("ERROR_ACCESS", {methodName});
            caller->returnError(mID, NS_BGMRPC::ERR_ACCESS,
                                m_name + '.' + methodName);
            caller->returnData(mID, QVariant());
            qWarning().noquote()
                << QString("Object(%1),access,Not allow(%2) call %1.%3")
                       .arg(m_name)
                       .arg(caller->m_ID)
                       .arg(mID);
        } else
            qWarning().noquote()
                << QString("(%1.%2),call,Caller may have disconnected")
                       .arg(m_name)
                       .arg(methodName);
    });

    QObject::connect(callThread, &QThread::finished, callThread,
                     &QThread::deleteLater);

    callThread->start();
}
