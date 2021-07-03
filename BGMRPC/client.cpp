#include "client.h"

#include <bgmrpccommon.h>

#include <QRegularExpression>

#include "bgmrpc.h"

using namespace NS_BGMRPC;

quint64 Client::m_totalID = 0;

Client::Client(BGMRPC* bgmrpc, QWebSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_BGMRPCSocket(socket) {
    m_ID = m_totalID;
    m_totalID++;

    QObject::connect(
        m_BGMRPCSocket, &QWebSocket::textMessageReceived,
        [=](const QString& message) { requestCall(message.toUtf8()); });
    QObject::connect(m_BGMRPCSocket, &QWebSocket::disconnected, this,
                     &Client::deleteLater);
    QObject::connect(m_BGMRPCSocket, &QWebSocket::disconnected, m_BGMRPCSocket,
                     &QWebSocket::deleteLater);

    m_BGMRPCSocket->ping();
    QObject::connect(m_BGMRPCSocket, &QWebSocket::pong, [=]() {
        QPointer<QWebSocket> socketPointer(m_BGMRPCSocket);
        QTimer::singleShot(1000, [=]() {
            if (socketPointer) socketPointer->ping();
        });
    });
}

Client::~Client() { m_relatedObjectSockets.clear(); }

bool
Client::operator==(const Client* other) const {
    return m_ID == other->m_ID;
}

bool
Client::operator==(quint64 cliID) const {
    return m_ID == cliID;
}

// quint64 Client::ID() const { return m_ID; }

QLocalSocket*
Client::connectObject(const QString& mID, const QString& objName) {
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

            /*QByteArray id(1, (quint8)DATA_CLIENTID);
            id.append(int2bytes<quint64>(m_ID));
            relSocket->write(id);*/

            QObject::connect(m_BGMRPCSocket, &QWebSocket::disconnected, [=]() {
                // relSocket->disconnectFromServer();
                qInfo().noquote() << QString(
                                         "Client(%1),disconnected,"
                                         "Disconnect from server")
                                         .arg(m_ID);
                if (m_relatedObjectSockets.contains(objName)) {
                    qInfo().noquote() << QString(
                                             "Client(%1),eisconnected,"
                                             "Disconnect object(%2)")
                                             .arg(m_ID)
                                             .arg(objName);
                    m_relatedObjectSockets[objName]->disconnectFromServer();
                } else
                    qWarning() << QString(
                                      "Client(%1),disconnected,"
                                      "related object(%2) may has crashed")
                                      .arg(m_ID)
                                      .arg(objName);
            });

            QObject::connect(relSocket, &QLocalSocket::disconnected, [=]() {
                m_relatedObjectSockets.remove(objName);
                relSocket->deleteLater();
            });

            QObject::connect(relSocket, &QLocalSocket::readyRead, [=]() {
                splitLocalSocketFragment(relSocket,
                                         [=](const QByteArray& readData) {
                                             qDebug() << "in client";
                                             returnData(readData);
                                         });
                /*int lenLen = sizeof(quint64);
                if (relSocket->property("fragment").isValid()) {
                    quint64 len =
                        relSocket->property("fragmentLen").toULongLong();
                    //                    QByteArray fragmentData =
                    // relSocket->property("fragmeng").toByteArray(); QByteArray
                readData = relSocket->read(len); quint64 readedLen =
                readData.length();

                    readData = relSocket->property("fragment").toByteArray() +
                               readData;

                    if (readedLen < len) {
                        relSocket->setProperty("fragment", readData);
                        relSocket->setProperty("fragmentLen", len - readedLen);

                        return;
                    } else {
                        returnData(readData);
                        relSocket->setProperty("fragment", QVariant());
                        relSocket->setProperty("fragmentLen", QVariant());
                    }
                }
                while (relSocket->bytesAvailable() > 0) {
                    quint64 len = bytes2int<quint64>(relSocket->read(lenLen));
                    QByteArray readData = relSocket->read(len);
                    quint64 readedLen = readData.length();
                    if (readedLen < len) {
                        relSocket->setProperty("fragment", readData);
                        relSocket->setProperty("fragmentLen", len - readedLen);
                    } else
                        returnData(readData);
                }*/
                /*QByteArray rawData = relSocket->readAll();

                splitData(rawData, [=](const QByteArray& retData) {
                    returnData(retData);
                });*/
            });

            return relSocket;
        } else {
            relSocket->deleteLater();
            return nullptr;
        }
    }
}

QLocalSocket*
Client::relatedObjectSocket(const QString& objName) const {
    return m_relatedObjectSockets[objName];
}

bool
Client::requestCall(const QByteArray& data) {
    //    if (data[0] > 30) {
    /*QJsonDocument jsonData = QJsonDocument::fromJson(data);
    QString objName = jsonData["object"].toString();
    QString mID = jsonData["mID"].toString();*/
    QVariantMap callVariant = QJsonDocument::fromJson(data).toVariant().toMap();

    QString objName = callVariant["object"].toString();
    QString mID = callVariant["mID"].toString();
    callVariant["callerID"] = m_ID;
    callVariant["callType"] = CALL_REMOTE;

    QByteArray callJson = QJsonDocument::fromVariant(callVariant).toJson();
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
            if (!relSocket) return false;
        }
        relSocket->write(int2bytes<quint64>(callJson.length()) + callJson);
        // relSocket->waitForBytesWritten();
        // relSocket->flush();
        return true;
    } else
        return false;
}

void
Client::returnData(const QByteArray& data) {
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
