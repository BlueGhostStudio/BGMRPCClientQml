#include "bgmrpcclient.h"
#include <QDebug>

using namespace NS_BGMRPCClient;

Calling::Calling(const QString& mID, QObject* parent)
    : QObject(parent), m_mID(mID) {}

void Calling::waitForReturn(CallChain* callChain, const BGMRPCClient* client) {
    QObject::connect(
        client, &BGMRPCClient::onReturn, [=](const QJsonDocument& jsonDoc) {
            if (jsonDoc["mID"].toString() == m_mID) {
                QObject::disconnect(client, &BGMRPCClient::onReturn, 0, 0);
                QObject::disconnect(client, &BGMRPCClient::onError, 0, 0);
                callChain->resolve(
                    jsonDoc["values"].toVariant() /*jsonDoc.toVariant()*/);
                deleteLater();
            }
        });
    QObject::connect(
        client, &BGMRPCClient::onError, [=](const QJsonDocument& jsonDoc) {
            QObject::disconnect(client, &BGMRPCClient::onReturn, 0, 0);
            QObject::disconnect(client, &BGMRPCClient::onError, 0, 0);
            callChain->reject(jsonDoc["error"].toVariant());
            deleteLater();
        });
}

// ===============

quint64 BGMRPCClient::m_totalMID = 0;
BGMRPCClient::BGMRPCClient(QObject* parent) : QObject(parent) {
    QObject::connect(&m_socket, &QWebSocket::connected, this,
                     &BGMRPCClient::connected);
    QObject::connect(&m_socket, &QWebSocket::disconnected, this,
                     &BGMRPCClient::disconnected);
    QObject::connect(&m_socket, &QWebSocket::textMessageReceived,
                     [=](const QString& message) {
                         QJsonDocument jsonDoc =
                             QJsonDocument::fromJson(message.toUtf8());
                         if (jsonDoc["type"].toString() == "return")
                             onReturn(jsonDoc);
                         else if (jsonDoc["type"].toString() == "signal")
                             onRemoteSignal(jsonDoc["object"].toString(),
                                            jsonDoc["signal"].toString(),
                                            jsonDoc["args"].toArray());
                         else if (jsonDoc["type"].toString() == "error")
                             onError(jsonDoc);
                     });
}

void BGMRPCClient::connectToHost(const QUrl& url) { m_socket.open(url); }

void BGMRPCClient::callMethod(CallChain* callChain, const QString& object,
                              const QString& method, const QVariantList& args) {
    m_mID = QString("#%1").arg(m_totalMID);
    m_totalMID++;
    QVariantMap callVariant;
    callVariant["object"] = object;
    callVariant["method"] = method;
    callVariant["args"] = args;
    callVariant["mID"] = m_mID;
    (new Calling(m_mID, this))->waitForReturn(callChain, this);
    m_socket.sendTextMessage(QJsonDocument::fromVariant(callVariant).toJson());
}
