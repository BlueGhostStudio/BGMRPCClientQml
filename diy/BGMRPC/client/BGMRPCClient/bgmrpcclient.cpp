#include "bgmrpcclient.h"

BGMRPCClient::BGMRPCClient(QObject* parent)
    : QObject (parent),
      ReturnBuffReady (false),
      MID (0)
{
    connect(&Socket, &QWebSocket::textMessageReceived,
            [=] (const QString& message) {
        QJsonDocument jsonDoc
                = QJsonDocument::fromJson(message.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.value("type").toString() == "signal")
            remoteSignal (jsonObj.value ("object").toString(),
                          jsonObj.value ("signal").toString(),
                          jsonObj.value ("args").toArray());
        else if (WaitReadLoop.isRunning()) {
            ReturnBuff = jsonDoc;
            ReturnBuffReady = true;
            WaitReadLoop.exit();
        }
    });

    connect(&Socket, &QWebSocket::connected,
            [=] () {
        if (WaitConnectLoop.isRunning())
            WaitConnectLoop.exit();
    });
}

void BGMRPCClient::setUrl(const QUrl& url)
{
    Url = url;
}

void BGMRPCClient::setUrl(const QString& url, int port)
{
    Url = QUrl (url);
    Url.setPort(port);
}

void BGMRPCClient::setProxy(const QNetworkProxy& proxy)
{
    Proxy = proxy;
}

void BGMRPCClient::connectToHost()
{
    QTimer::singleShot(10000, &WaitConnectLoop, SLOT(quit()));
    Socket.open(Url);
    WaitConnectLoop.exec();
}

QVariant BGMRPCClient::callMethod(const QString& obj,
                                       const QString& method,
                                       const QJsonArray& args)
{
    if (!Socket.isValid())
        return QVariant ();
    ReturnBuffReady = false;
    ReturnBuff = QJsonDocument ();

    // TODO call remote api
    QJsonObject callJson;
    callJson ["method"] = method;
    callJson ["args"] = args;
    callJson ["object"] = obj;
    callJson ["mID"] = MID;
    MID++;

    Socket.sendTextMessage(QJsonDocument (callJson).toJson());

    QJsonArray jsRet = readReturn ().object ()["values"].toArray ();
    if (jsRet.size () == 1)
        return jsRet[0].toVariant ();
    else if (jsRet.size () > 1)
        return jsRet.toVariantList ();
    else
        return QVariant ();
}

QJsonDocument BGMRPCClient::readReturn()
{
    QTimer::singleShot (10000, &WaitReadLoop, SLOT(quit()));
    WaitReadLoop.exec ();

    return ReturnBuff;
}
