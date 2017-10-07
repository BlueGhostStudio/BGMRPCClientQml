#include "bgmrpcclient.h"

Calling::Calling (QObject* parent)
    : QObject (parent)
{

}

QJsonDocument Calling::returnData () {
    QTimer::singleShot (10000, &WaitReadLoop, SLOT(quit()));
    WaitReadLoop.exec ();

    return ReturnBuff;
}

void Calling::readyRead (const QJsonDocument& data) {
    ReturnBuff = data;
    WaitReadLoop.exit ();
}

BGMRPCClient::BGMRPCClient (QObject* parent)
    : QObject (parent),
      MID (0)
{
    connect (&Socket, &QWebSocket::textMessageReceived,
             [=] (const QString& message) {
        QJsonDocument jsonDoc
                = QJsonDocument::fromJson(message.toUtf8());
        QJsonObject jsonObj = jsonDoc.object();
        if (jsonObj.value("type").toString() == "signal")
            remoteSignal (jsonObj.value ("object").toString(),
                          jsonObj.value ("signal").toString(),
                          jsonObj.value ("args").toArray());
        else {
            QString mID = jsonObj.value ("mID").toString ();
            Calling* theCalling = Callings.take (mID);
            theCalling->deleteLater ();
            theCalling->readyRead (jsonDoc);
        }
    });

    connect (&Socket, &QWebSocket::connected,
            [=] () {
        if (WaitConnectLoop.isRunning())
            WaitConnectLoop.exit();

        emit connected ();
    });
    connect (&Socket, &QWebSocket::disconnected,
             this, &BGMRPCClient::disconnected);
    connect (&Socket, &QWebSocket::stateChanged,
             this, &BGMRPCClient::stateChanged);
}

void BGMRPCClient::setUrl (const QUrl& url)
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

QVariant BGMRPCClient::callMethod(const QString& obj,
                                       const QString& method,
                                       const QJsonArray& args)
{
    if (!Socket.isValid())
        return QVariant ();

    QString _MID = QString::number (MID);
    // TODO call remote api
    QJsonObject callJson;
    callJson ["method"] = method;
    callJson ["args"] = args;
    callJson ["object"] = obj;
    callJson ["mID"] = _MID;

    Calling* newCall = new Calling;
    Callings[_MID] = newCall;

    MID++;

    Socket.sendTextMessage(QJsonDocument (callJson).toJson());

    QJsonArray jsRet = newCall->returnData ().object ()["values"].toArray ();
    if (jsRet.size () == 1)
        return jsRet[0].toVariant ();
    else if (jsRet.size () > 1)
        return jsRet.toVariantList ();
    else
        return QVariant ();

    /*QJsonArray jsRet = readReturn ().object ()["values"].toArray ();
    if (jsRet.size () == 1)
        return jsRet[0].toVariant ();
    else if (jsRet.size () > 1)
        return jsRet.toVariantList ();
    else
        return QVariant ();*/
}

QAbstractSocket::SocketState BGMRPCClient::state() const
{
    return Socket.state ();
}

void BGMRPCClient::connectToHost()
{
    QTimer::singleShot(10000, &WaitConnectLoop, SLOT(quit()));
    Socket.open(Url);
    WaitConnectLoop.exec();
}

void BGMRPCClient::close()
{
    Socket.close ();
}

/*QJsonDocument BGMRPCClient::readReturn()
{
    QTimer::singleShot (10000, &WaitReadLoop, SLOT(quit()));
    WaitReadLoop.exec ();

    return ReturnBuff;
}*/
