#include "client.h"

#include <bgmrpccommon.h>

#include <QRegularExpression>

#include "bgmrpc.h"
// #include "objectplug.h"

using namespace NS_BGMRPC;

quint64 Client::m_totalID = 0;

Client::Client(BGMRPC* bgmrpc, QWebSocket* socket, QObject* parent)
    : QObject(parent), m_BGMRPC(bgmrpc), m_BGMRPCCliSlot(socket) {
    m_ID = m_totalID;
    m_totalID++;

    QObject::connect(
        m_BGMRPCCliSlot, &QWebSocket::textMessageReceived,
        [=](const QString& message) { requestCall(message.toUtf8()); });

    QObject::connect(m_BGMRPCCliSlot, &QWebSocket::disconnected, this, [&]() {
        qInfo().noquote() << QString("Client(%1), disconnected").arg(m_ID);
        QMap<QString, QLocalSocket*>::iterator it;
        for (it = m_dataConnecters.begin(); it != m_dataConnecters.end();
             ++it) {
            qInfo().noquote()
                << QString("Disconnect Related Object(%1)").arg(it.key());
            it.value()->disconnectFromServer();
            it.value()->deleteLater();
        }
        m_dataConnecters.clear();

        m_BGMRPCCliSlot->deleteLater();
        deleteLater();
    });

    m_BGMRPCCliSlot->ping();
    QObject::connect(m_BGMRPCCliSlot, &QWebSocket::pong, [=]() {
        QPointer<QWebSocket> socketPointer(m_BGMRPCCliSlot);
        QTimer::singleShot(1000, [=]() {
            if (socketPointer) socketPointer->ping();
        });
    });
}

Client::~Client() {}

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
    if (!m_BGMRPC->checkObject(objName.toLatin1()))
        return nullptr;
    else {
        QLocalSocket* dataConnecter = new QLocalSocket(this);
        //        relSocket->setObjectName(objName);
        dataConnecter->connectToServer(BGMRPCObjPrefix + objName);
        if (dataConnecter->waitForConnected()) {
            m_dataConnecters[objName] = dataConnecter;

            QObject::connect(
                dataConnecter, &QLocalSocket::readyRead, this, [=]() {
                    splitLocalSocketFragment(dataConnecter,
                                             [=](const QByteArray& readData) {
                                                 returnData(readData);
                                             });
                });

            return dataConnecter;
        } else {
            dataConnecter->deleteLater();
            return nullptr;
        }
    }
}

bool
Client::requestCall(const QByteArray& data) {
    QJsonObject jsoCall = QJsonDocument::fromJson(data).object();

    QString objName = jsoCall["object"].toString();
    QString mID = jsoCall["mID"].toString();
    jsoCall["callerID"] = (qint64)m_ID;
    jsoCall["callType"] = CALL_REMOTE;

    if (!objName.isEmpty()) {
        QLocalSocket* dataConnecter =
            m_dataConnecters.contains(objName)
                ? m_dataConnecters[objName]
                : nullptr;  // this->dataConnecter(objName);
        if (!dataConnecter) {
            dataConnecter = connectObject(mID, objName);
            if (!dataConnecter) {
                if (m_BGMRPC->checkObject("MissingObjectHandler")) {
                    QJsonArray args{ jsoCall["object"], jsoCall["method"],
                                     jsoCall["args"] };
                    jsoCall["object"] = "MissingObjectHandler";
                    jsoCall["method"] = "request";
                    jsoCall["args"] = args;

                    return requestCall(
                        QJsonDocument(jsoCall).toJson(QJsonDocument::Compact));
                } else {
                    QJsonObject errJsonObj;
                    errJsonObj["type"] = "error";
                    errJsonObj["errNo"] = ERR_NOOBJ;
                    errJsonObj["mID"] = mID;
                    errJsonObj["error"] =
                        QString("No exist object the name is %1").arg(objName);

                    returnData(QJsonDocument(errJsonObj)
                                   .toJson(QJsonDocument::Compact));
                    return false;
                }
            }
        }

        QByteArray callJson =
            QJsonDocument(jsoCall).toJson(QJsonDocument::Compact);
        dataConnecter->write(int2bytes<quint64>(callJson.length()) + callJson);

        return true;
    } else
        return false;
}

void
Client::returnData(const QByteArray& data) {
    m_BGMRPCCliSlot->sendTextMessage(data);
}
