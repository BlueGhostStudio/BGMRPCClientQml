#include "client.h"

#include <bgmrpccommon.h>

#include <QRegularExpression>

#include "bgmrpc.h"
//#include "objectplug.h"

using namespace NS_BGMRPC;

quint64 Client::m_totalID = 0;

Client::Client(BGMRPC* bgmrpc, QWebSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_BGMRPCCliSlot(socket) {
    m_ID = m_totalID;
    m_totalID++;

    QObject::connect(
        m_BGMRPCCliSlot, &QWebSocket::textMessageReceived,
        [=](const QString& message) { requestCall(message.toUtf8()); });
    QObject::connect(m_BGMRPCCliSlot, &QWebSocket::disconnected, this,
                     &Client::deleteLater);
    QObject::connect(m_BGMRPCCliSlot, &QWebSocket::disconnected, m_BGMRPCCliSlot,
                     &QWebSocket::deleteLater);

    m_BGMRPCCliSlot->ping();
    QObject::connect(m_BGMRPCCliSlot, &QWebSocket::pong, [=]() {
        QPointer<QWebSocket> socketPointer(m_BGMRPCCliSlot);
        QTimer::singleShot(1000, [=]() {
            if (socketPointer) socketPointer->ping();
        });
    });
}

Client::~Client() { m_dataConnecters.clear(); }

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
    /*ObjectPlug* objCtrl = m_BGMRPC->objectCtrl(objName);
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
    }*/
    if (!m_BGMRPC->checkObject(objName.toLatin1())) {
        QJsonObject errJsonObj;
        errJsonObj["type"] = "error";
        errJsonObj["errNo"] = ERR_NOOBJ;
        errJsonObj["mID"] = mID;
        errJsonObj["error"] =
            QString("No exist object the name is %1").arg(objName);

        returnData(QJsonDocument(errJsonObj).toJson(QJsonDocument::Compact));

        return nullptr;
    } else {
        QLocalSocket* dataConnecter = new QLocalSocket(this);
        //        relSocket->setObjectName(objName);
        dataConnecter->connectToServer(BGMRPCObjPrefix + objName);
        if (dataConnecter->waitForConnected()) {
            m_dataConnecters[objName] = dataConnecter;

            QObject::connect(m_BGMRPCCliSlot, &QWebSocket::disconnected, [=]() {
                // relSocket->disconnectFromServer();
                qInfo().noquote() << QString(
                                         "Client(%1),disconnected,"
                                         "Disconnect from server")
                                         .arg(m_ID);
                if (m_dataConnecters.contains(objName)) {
                    qInfo().noquote() << QString(
                                             "Client(%1),eisconnected,"
                                             "Disconnect object(%2)")
                                             .arg(m_ID)
                                             .arg(objName);
                    m_dataConnecters[objName]->disconnectFromServer();
                } else
                    qWarning() << QString(
                                      "Client(%1),disconnected,"
                                      "related object(%2) may has crashed")
                                      .arg(m_ID)
                                      .arg(objName);
            });

            QObject::connect(dataConnecter, &QLocalSocket::disconnected, this,
                             [=]() {
                                 m_dataConnecters.remove(objName);
                                 dataConnecter->deleteLater();
                             });

            QObject::connect(dataConnecter, &QLocalSocket::readyRead, this, [=]() {
                splitLocalSocketFragment(
                    dataConnecter,
                    [=](const QByteArray& readData) { returnData(readData); });
            });

            return dataConnecter;
        } else {
            dataConnecter->deleteLater();
            return nullptr;
        }
    }
}

/*QLocalSocket*
Client::dataConnecter(const QString& objName) const {
    return m_dataConnecters[objName];
}*/

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
        QLocalSocket* dataConnecter = m_dataConnecters[objName];//this->dataConnecter(objName);
        if (!dataConnecter) {
            dataConnecter = connectObject(mID, objName);
            if (!dataConnecter) return false;
        }
        dataConnecter->write(int2bytes<quint64>(callJson.length()) + callJson);
        // relSocket->waitForBytesWritten();
        // relSocket->flush();
        return true;
    } else
        return false;
}

void
Client::returnData(const QByteArray& data) {
    m_BGMRPCCliSlot->sendTextMessage(data);
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
