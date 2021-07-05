#include "caller.h"

#include <intbytes.h>

#include "objectinterface.h"

using namespace NS_BGMRPCObjectInterface;

// quint64 Caller::m_totalID = 0;

Caller::Caller(ObjectInterface* callee, QLocalSocket* socket, QObject* parent)
    : QObject(parent), m_dataSocket(socket), m_callee(callee) {
    //    m_localCall = false;
    m_ID = -1;
    m_exited = false;
    m_callType = NS_BGMRPC::CALL_UNDEFINED;

    QObject::connect(m_dataSocket, &QLocalSocket::disconnected, [=]() {
        m_exited = true;
        deleteLater();
    });
    QObject::connect(m_dataSocket, &QLocalSocket::disconnected, m_dataSocket,
                     &QLocalSocket::deleteLater);
    QObject::connect(m_dataSocket, &QLocalSocket::disconnected,
                     [=]() { clientExited(); });

    QObject::connect(this, &Caller::emitSignal, this, &Caller::onEmitSignal);
    QObject::connect(this, &Caller::returnData, this, &Caller::onReturnData);
    QObject::connect(this, &Caller::returnError, this, &Caller::onReturnError);
    //    m_ID = m_totalID;
    //    m_totalID++;
}

Caller::~Caller() {}

qint64
Caller::ID() const {
    return m_ID;
}

bool
Caller::exited() const {
    return m_exited;
}

bool
Caller::isInternalCall() const {
    return m_callType == NS_BGMRPC::CALL_INTERNAL ||
           m_callType == NS_BGMRPC::CALL_INTERNAL_NOBLOCK;
}

QString
Caller::app() const {
    return m_callerApp;
}

QString
Caller::object() const {
    return m_callerObject;
}

QString
Caller::grp() const {
    return m_callerGrp;
}

// void Caller::setID(quint64 id) { m_ID = id; }

void
Caller::onReturnData(const QString& mID, const QVariant& data) {
    if (!m_dataSocket) return;

    QJsonObject retJsonObj;
    retJsonObj["type"] = "return";
    retJsonObj["mID"] = mID;
    QJsonValue retJsonValue = data.toJsonValue();
    if (retJsonValue.isArray())
        retJsonObj["values"] = retJsonValue;
    else {
        retJsonObj["values"] = QJsonArray({ retJsonValue });
    }
    QByteArray retData =
        QJsonDocument(retJsonObj).toJson(QJsonDocument::Compact);

    qInfo().noquote() << QString(
                             "Object(%1),returnData,%1.%2 return data.Size: %3")
                             .arg(m_callee->objectName())
                             .arg(m_calleeMethod)
                             .arg(retData.length());

    m_dataSocket->write(int2bytes<quint64>(retData.length()) + retData);
    m_dataSocket->flush();
}

void
Caller::onEmitSignal(const QString& signal, const QVariant& args) {
    //    if (m_localCall || !m_dataSocket) return;
    if (m_callType == NS_BGMRPC::CALL_INTERNAL ||
        m_callType == NS_BGMRPC::CALL_INTERNAL_NOBLOCK || !m_dataSocket)
        return;

    QJsonObject signalJsonObj;
    signalJsonObj["type"] = "signal";
    signalJsonObj["object"] = m_callee->objectName();
    signalJsonObj["signal"] = signal;
    QJsonValue argsJsonValue = args.toJsonValue();
    if (argsJsonValue.isArray())
        signalJsonObj["args"] = argsJsonValue;
    else
        signalJsonObj["args"] = QJsonArray({ argsJsonValue });

    QByteArray signalData =
        QJsonDocument(signalJsonObj).toJson(QJsonDocument::Compact);

    qInfo().noquote()
        << QString("Object(%1),emitSignal,Emit signal(%2) to Client(%3)")
               .arg(m_callee->objectName())
               .arg(signal)
               .arg(m_ID);

    m_dataSocket->write(int2bytes<quint64>(signalData.length()) + signalData);
    m_dataSocket->flush();
}

void
Caller::onReturnError(const QString& mID, quint8 errNO, const QString& errStr) {
    if (!m_dataSocket) return;
    //    QByteArray errData(2, '\x0');
    //    errData[0] = (quint8) NS_BGMRPC::DATA_ERROR;
    //    errData[1] = errNO;
    QJsonObject errJsonObj;
    errJsonObj["type"] = "error";
    errJsonObj["errNo"] = errNO;
    errJsonObj["mID"] = mID;
    errJsonObj["error"] = errStr;

    switch (errNO) {
    case NS_BGMRPC::ERR_NOMETHOD:
        errJsonObj["error"] = QString("No exist %1 method").arg(errStr);
        break;
    case NS_BGMRPC::ERR_ACCESS:
        errJsonObj["error"] =
            QString("This client is not allowed to call the %1 method")
                .arg(errStr);
        break;
    }

    QByteArray errData =
        QJsonDocument(errJsonObj).toJson(QJsonDocument::Compact);

    //    errData.append(errStr);
    m_dataSocket->write(int2bytes<quint64>(errData.length()) + errData);
    m_dataSocket->flush();
}

void
Caller::unsetDataSocket() {
    QObject::disconnect(m_dataSocket, &QLocalSocket::disconnected, 0, 0);
    m_dataSocket->disconnectFromServer();
    m_dataSocket->deleteLater();
    m_dataSocket = nullptr;
}
