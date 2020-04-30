#include "client.h"
#include "bgmrpc.h"
#include <bgmrpccommon.h>

#include <QRegularExpression>

using namespace NS_BGMRPC;

quint64 Client::m_totalID = 0;

Client::Client(BGMRPC* bgmrpc, QWebSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_BGMRPCSocket(socket)
{
    m_ID = m_totalID;
    m_totalID++;

    QObject::connect(
        m_BGMRPCSocket, &QWebSocket::textMessageReceived,
        [=](const QString& message) { requestCall(message.toUtf8()); });
}

Client::~Client()
{
    m_relatedObjectSockets.clear();
}

bool Client::operator==(const Client* other) const
{
    return m_ID == other->m_ID;
}

bool Client::operator==(quint64 cliID) const
{
    return m_ID == cliID;
}

// quint64 Client::ID() const { return m_ID; }

QLocalSocket* Client::connectObject(const QString& objName)
{
    ObjectCtrl* objCtrl = m_BGMRPC->objectCtrl(objName);
    if (!objCtrl) {
        returnError(ERR_NOOBJ, objName);
        return nullptr;
    } else {
        QLocalSocket* relSocket = new QLocalSocket(this);
        //        relSocket->setObjectName(objName);
        relSocket->connectToServer(objCtrl->dataSocketName());
        if (relSocket->waitForConnected()) {
            m_relatedObjectSockets[objName] = relSocket;

            QByteArray id(1, (quint8)DATA_CLIENTID);
            id.append(int2bytes<quint64>(m_ID));
            relSocket->write(id);

            QObject::connect(m_BGMRPCSocket, &QWebSocket::disconnected,
                             [=]() { relSocket->disconnectFromServer(); });

            QObject::connect(relSocket, &QLocalSocket::disconnected,
                             [=]() { relSocket->deleteLater(); });

            QObject::connect(relSocket, &QLocalSocket::readyRead, [=]() {
                QByteArray rawData = relSocket->readAll();

                splitReturnData(rawData, [=](const QByteArray& retData) {
                    if (retData[0] == DATA_ERROR)
                        returnError((int)retData[1], retData.mid(2));
                    else
                        returnData(retData);
                });
            });

            return relSocket;
        } else {
            relSocket->deleteLater();
            return nullptr;
        }
    }
}

QLocalSocket* Client::relatedObjectSocket(const QString& objName) const
{
    return m_relatedObjectSockets[objName];
}

bool Client::requestCall(const QByteArray& data)
{
    //    if (data[0] > 30) {
    QRegularExpression re(R"RX(^{[^{]*"object":\s*"([^\"]*)")RX");

    QRegularExpressionMatch match = re.match(data);

    if (match.hasMatch()) {
        QString objName = match.captured(1);
        QLocalSocket* relSocket = relatedObjectSocket(objName);
        if (!relSocket) {
            relSocket = connectObject(objName);
            if (!relSocket)
                return false;
        }
        relSocket->write(data);
        return true;
    } else
        return false;
}

void Client::returnData(const QByteArray& data)
{
    m_BGMRPCSocket->sendTextMessage(data);
}

void Client::returnError(quint8 errNo, const QString& errStr)
{
    QJsonObject errJsonObj;
    errJsonObj["type"] = "error";
    errJsonObj["errno"] = errNo;
    switch (errNo) {
    case NS_BGMRPC::ERR_NOOBJ:
        errJsonObj["error"] =
            QString("The remote object [%1] does not exist").arg(errStr);
        break;
    case NS_BGMRPC::ERR_NOMETHOD:
        errJsonObj["error"] = QString("%1 no found").arg(errStr);
        break;
    }
    //    errJsonObj["error"] = errStr;

    m_BGMRPCSocket->sendTextMessage(
        QJsonDocument(errJsonObj).toJson(QJsonDocument::Compact));
}
