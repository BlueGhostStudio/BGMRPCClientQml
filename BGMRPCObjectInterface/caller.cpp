#include "caller.h"
#include "objectinterface.h"
#include <bgmrpccommon.h>
#include <intbytes.h>

using namespace NS_BGMRPCObjectInterface;

// quint64 Caller::m_totalID = 0;

Caller::Caller(ObjectInterface* callee, QLocalSocket* socket, QObject* parent)
    : QObject(parent), m_dataSocket(socket), m_callee(callee) {
    m_localCall = false;
    m_ID = -1;
    QObject::connect(m_dataSocket, &QLocalSocket::disconnected, this,
                     &Caller::deleteLater);
    QObject::connect(m_dataSocket, &QLocalSocket::disconnected, m_dataSocket,
                     &QLocalSocket::deleteLater);
    QObject::connect(m_dataSocket, &QLocalSocket::disconnected,
                     [=]() { clientExited(); });
    //    m_ID = m_totalID;
    //    m_totalID++;
}

qint64 Caller::ID() const { return m_ID; }

// void Caller::setID(quint64 id) { m_ID = id; }

void Caller::returnData(const QString& mID, const QVariant& data) {
    QJsonObject retJsonObj;
    retJsonObj["type"] = "return";
    retJsonObj["mID"] = mID;
    QJsonValue retJsonValue = data.toJsonValue();
    if (retJsonValue.isArray())
        retJsonObj["values"] = retJsonValue;
    else {
        retJsonObj["values"] = QJsonArray({retJsonValue});
    }
    QByteArray retData =
        QJsonDocument(retJsonObj).toJson(QJsonDocument::Compact);
    m_dataSocket->write(int2bytes<quint64>(retData.length()) + retData);
    m_dataSocket->flush();
}

void Caller::emitSignal(const QString& signal, const QVariant& args) {
    if (m_localCall)
        return;

    QJsonObject signalJsonObj;
    signalJsonObj["type"] = "signal";
    signalJsonObj["object"] = m_callee->objectName();
    signalJsonObj["signal"] = signal;
    QJsonValue argsJsonValue = args.toJsonValue();
    if (argsJsonValue.isArray())
        signalJsonObj["args"] = argsJsonValue;
    else
        signalJsonObj["args"] = QJsonArray({argsJsonValue});

    QByteArray signalData =
        QJsonDocument(signalJsonObj).toJson(QJsonDocument::Compact);
    m_dataSocket->write(int2bytes<quint64>(signalData.length()) + signalData);
    m_dataSocket->flush();
}

void Caller::returnError(quint8 errNO, const QString& errStr) {
    QByteArray errData(2, '\x0');
    errData[0] = (quint8)NS_BGMRPC::DATA_ERROR;
    errData[1] = errNO;
    errData.append(errStr);
    m_dataSocket->write(int2bytes<quint64>(errData.length()) + errData);
    m_dataSocket->flush();
}
