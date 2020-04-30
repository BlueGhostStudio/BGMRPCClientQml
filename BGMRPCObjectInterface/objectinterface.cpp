#include "objectinterface.h"
#include "caller.h"

#include <QPointer>
#include <bgmrpccommon.h>

using namespace NS_BGMRPCObjectInterface;

ObjectInterface::ObjectInterface(QObject* parent)
    : QObject(parent), m_ctrlSocket(new QLocalSocket(this)),
      m_dataServer(new QLocalServer(this))
{

    m_ctrlSocket->connectToServer(NS_BGMRPC::BGMRPCCtrlSocket);
    if (m_ctrlSocket->waitForConnected(-1))
        qInfo() << "Connect to BGMRPC ok";
    else
        qWarning() << "Connect to BGMRPC Fail";

    QObject::connect(m_ctrlSocket, &QLocalSocket::disconnected, [&]() {
        qInfo() << "Object disconnect";
        objectDisconnected();
        m_dataServer->close();
    });

    QObject::connect(m_dataServer, &QLocalServer::newConnection, this,
                     &ObjectInterface::newCaller);
}

bool ObjectInterface::registerObject(const QByteArray& name)
{
    if (!m_ctrlSocket->isOpen()) {
        //        qWarning() << "Register fail";
        return false;
    } else {
        QByteArray ctrl(1, (quint8)NS_BGMRPC::CTRL_REGISTER);
        //        ctrl[0] = (quint8)NS_BGMRPC::CTRL_REGISTER;
        ctrl.append(name);
        m_ctrlSocket->write(ctrl);

        QString socketServerName = NS_BGMRPC::BGMRPCObjPrefix + name;
        if (m_dataServer->listen(socketServerName)) {
            m_name = name;
            qInfo() << "Object ready";
            return true;
        } else {
            qWarning() << "Object Not ready. Object Name:" << name
                       << ", Socket Name:" << socketServerName;
            return false;
        }
    }
}

QString ObjectInterface::objectName() const
{
    return m_name;
}

/*!
 * \brief 将调用者(客户端)与对象关联
 * \param caller 调用者
 * \note 将客户端关联对象，其目的在于：
 * + 对象在广播信号时只向其所关联的客户端发送信号，而非所有客户端
 * + 对象可在关联客户端中查找
 */
void ObjectInterface::addRelatedCaller(QPointer<Caller> caller)
{
    m_relatedCaller[caller->m_ID] = caller;
    QObject::connect(caller.data(), &Caller::clientExited, [=]() {
        relatedCallerExisted(caller);
        m_relatedCaller.remove(caller->m_ID);
    });
}

void ObjectInterface::emitSignal(const QString& signal, const QVariant& args)
{
    foreach (QPointer<Caller> caller, m_relatedCaller)
        caller->emitSignal(signal, args);
}

QPointer<Caller> ObjectInterface::findRelatedCaller(
    std::function<bool(QPointer<Caller>)> callback)
{
    foreach (QPointer<Caller> caller, m_relatedCaller) {
        if (callback(caller))
            return caller;
    }

    return QPointer<Caller>();
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
                                          const QVariantList& args)
{
    QByteArray checkObj(1, (quint8)NS_BGMRPC::CTRL_CHECKOBJECT);
    checkObj.append(object);
    m_ctrlSocket->write(checkObj);
    if (!m_ctrlSocket->waitForBytesWritten()) {
        qWarning() << "Call local method - Can't request check object";
        return QVariant();
    }
    if (!m_ctrlSocket->waitForReadyRead() ||
        !(quint8)(m_ctrlSocket->readAll()[0])) {
        qWarning() << "Call local method - The requested object [" << object
                   << "] does not exist";
        return QVariant();
    }

    QVariantMap callVariant;
    callVariant["object"] = object;
    callVariant["method"] = method;
    callVariant["args"] = args;
    QLocalSocket* localCallSocket = new QLocalSocket();
    localCallSocket->connectToServer(NS_BGMRPC::BGMRPCObjPrefix + object);
    if (!localCallSocket->waitForConnected()) {
        localCallSocket->deleteLater();
        return QVariant();
    }

    QByteArray callData;
    if (!caller.isNull()) {
        callData.resize(1);
        callData[0] = (quint8)NS_BGMRPC::DATA_LOCALCALL_CLIENTID;
        callData.append(int2bytes<quint64>(caller->m_ID));
    }

    callData += QJsonDocument::fromVariant(callVariant).toJson();
    localCallSocket->write(callData);
    if (!localCallSocket->waitForBytesWritten()) {
        localCallSocket->deleteLater();
        return QVariant();
    }

    QVariant retVariant;
    bool capturedReturn = false;
    while (localCallSocket->waitForReadyRead()) {
        NS_BGMRPC::splitReturnData(
            localCallSocket->readAll(), [&](const QByteArray& baRetData) {
                QJsonDocument jsonDoc = QJsonDocument::fromJson(baRetData);
                if (jsonDoc["type"].toString() == "return") {
                    capturedReturn = true;
                    retVariant = jsonDoc["values"].toVariant();
                }
            });
        if (capturedReturn)
            break;
    }
    localCallSocket->deleteLater();

    return retVariant;
}

/*!
 * \overload
 * \note
 * 忽略调用者参数，如果被调用的远端对象无需操作原始调用者客户端(例如私有数据)，则调用此成员函数
 */
QVariant ObjectInterface::callLocalMethod(const QString& object,
                                          const QString& method,
                                          const QVariantList& args)
{
    return callLocalMethod(nullptr, object, method, args);
}

void ObjectInterface::setPrivateData(QPointer<Caller> caller,
                                     const QString& name, const QVariant& data)
{
    quint64 cliID = caller->m_ID;
    if (!m_privateDatas.contains(cliID))
        m_privateDatas[cliID] = QVariantMap();

    m_privateDatas[cliID][name] = data;
}

QVariant ObjectInterface::privateData(QPointer<Caller> caller,
                                      const QString& name) const
{
    quint64 cliID = caller->m_ID;
    if (!m_privateDatas.contains(cliID))
        return QVariant();

    return m_privateDatas[cliID][name];
}

void ObjectInterface::newCaller()
{
    qInfo() << "New call this object[" << m_name << "]";
    QLocalSocket* cliDataSocket = m_dataServer->nextPendingConnection();
    QPointer<Caller> caller = new Caller(this, cliDataSocket);

    QObject::connect(cliDataSocket, &QLocalSocket::readyRead, [=]() {
        QByteArray data = cliDataSocket->readAll();
        quint8 firstCh = (quint8)data[0];
        if (firstCh == NS_BGMRPC::DATA_CLIENTID ||
            firstCh == NS_BGMRPC::DATA_LOCALCALL_CLIENTID) {
            quint64 id = bytes2int<quint64>(data.mid(1, sizeof(quint64)));
            if (firstCh == NS_BGMRPC::DATA_LOCALCALL_CLIENTID)
                caller->m_localCall = true;
            caller->m_ID = id;
            data = data.mid(sizeof(quint64) + 1);
            if (data.length() == 0)
                return;
        }
        QJsonDocument callJsonDoc = QJsonDocument::fromJson(data);

        QString methodName = callJsonDoc["method"].toString();
        QVariantList args = callJsonDoc["args"].toVariant().toList();
        QString mID = callJsonDoc["mID"].toString("#");

        if (!m_methods[methodName])
            caller->returnError(NS_BGMRPC::ERR_NOMETHOD,
                                m_name + '.' + methodName);
        else
            callMethod(mID, caller, methodName, args);
        /*else {
            if (permit(caller, methodName, args))
                callMethod(mID, caller, methodName, args);
            else {
                caller->emitSignal("ERROR_ACCESS", {methodName});
                caller->returnData(mID, QVariant());
            }
        }*/ /* else {
              QVariant ret = m_methods[methodName](this, caller, args);
              caller->returnData(mID, ret);
          }*/
    });

    QObject::connect(caller.data(), &Caller::clientExited, [=]() {
        callerExisted(caller);
        m_privateDatas.remove(caller->m_ID);
    });
}

bool ObjectInterface::permit(QPointer<Caller> /*caller*/,
                             const QString& /*method*/,
                             const QVariantList& /*args*/)
{
    return true;
}

void ObjectInterface::callMethod(const QString& mID, QPointer<Caller> caller,
                                 const QString& methodName,
                                 const QVariantList& args)
{
    if (!permit(caller, methodName, args)) {
        caller->emitSignal("ERROR_ACCESS", {methodName});
        caller->returnData(mID, QVariant());
        return;
    }
    qInfo() << m_name + '.' + methodName << " has been called";
    QVariantList _args_(args);
    for (int i = args.length(); i < 10; i++)
        _args_ << QVariant();

    QVariant ret = m_methods[methodName](this, caller, _args_);
    qWarning() << "Not allow call " + methodName;

    caller->returnData(mID, ret);
}
