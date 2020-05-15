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
    QObject::connect(m_BGMRPCSocket, &QWebSocket::disconnected, this,
                     &Client::deleteLater);
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

QLocalSocket* Client::connectObject(const QString& mID, const QString& objName)
{
    ObjectCtrl* objCtrl = m_BGMRPC->objectCtrl(objName);
    if (!objCtrl) {
        QJsonObject errJsonObj;
        errJsonObj["type"] = "error";
        errJsonObj["errNo"] = ERR_NOOBJ;
        errJsonObj["mID"] = mID;
        errJsonObj["error"] =
            QString("No exist object the name is %1").arg(objName);

        returnData(QJsonDocument(errJsonObj).toJson(QJsonDocument::Compact));

        //        returnError(ERR_NOOBJ, objName);
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

            QObject::connect(m_BGMRPCSocket, &QWebSocket::disconnected, [=]() {
                // relSocket->disconnectFromServer();
                qInfo().noquote() << "Client disconnect from server";
                if (m_relatedObjectSockets.contains(objName)) {
                    qInfo().noquote()
                        << "Client disconnect object[" + objName + "]";
                    m_relatedObjectSockets[objName]->disconnectFromServer();
                } else
                    qWarning() << "The object[" + objName +
                                      "] related with this client may "
                                      "have crashed";
            });

            QObject::connect(relSocket, &QLocalSocket::disconnected, [=]() {
                m_relatedObjectSockets.remove(objName);
                relSocket->deleteLater();
            });

            QObject::connect(relSocket, &QLocalSocket::readyRead, [=]() {
                QByteArray rawData = relSocket->readAll();

                splitReturnData(rawData, [=](const QByteArray& retData) {
                    /*if (retData[0] == DATA_ERROR)
                        returnError((int)retData[1], retData.mid(2));
                    else*/
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
    QJsonDocument jsonData = QJsonDocument::fromJson(data);
    QString objName = jsonData["object"].toString();
    QString mID = jsonData["mID"].toString();
    /*QRegularExpression reObj(R"RX(^{[^{]*"object":\s*"([^\"]*)")RX");
    QRegularExpressionMatch matchObj = reObj.match(data);

    QRegularExpression reMID(R"RX(^{[^{]*"mID":\s*"([^\"]*))RX");
    QRegularExpressionMatch matchMID = reMID.match(data);*/

    if (!objName.isEmpty()) {
        //        QString objName = matchObj.captured(1);
        //        QString mID = matchMID.captured(1);
        QLocalSocket* relSocket = relatedObjectSocket(objName);
        if (!relSocket) {
            relSocket = connectObject(mID, objName);
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

/*void Client::returnError(quint8 errNo, const QString& errStr)
{
    QJsonObject errJsonObj;
    errJsonObj["type"] = "error";
    errJsonObj["errno"] = errNo;
    switch (errNo) {
    case NS_BGMRPC::ERR_NOOBJ:
        errJsonObj["info"] =
            QString("The remote object [%1] does not exist").arg(errStr);
        break;
    case NS_BGMRPC::ERR_NOMETHOD:
        errJsonObj["info"] = QString("%1 no found").arg(errStr);
        break;
    case NS_BGMRPC::ERR_ACCESS:
        errJsonObj["info"] = QString("%1 not allow call").arg(errStr);
        break;
    }
    //    errJsonObj["error"] = errStr;

    m_BGMRPCSocket->sendTextMessage(
        QJsonDocument(errJsonObj).toJson(QJsonDocument::Compact));
}*/
