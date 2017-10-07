#ifndef BGMRPCCLIENT_H
#define BGMRPCCLIENT_H

#include <QtWebSockets>
#include <QHostAddress>
#include <QObject>
#include <QNetworkProxy>
#include <QtCore>

class Calling : public QObject {
    Q_OBJECT

public:
    Calling (QObject* parent=nullptr);

    QJsonDocument returnData ();

public slots:
    void readyRead (const QJsonDocument& data);

private:
    QJsonDocument ReturnBuff;
    QEventLoop WaitReadLoop;
};

class BGMRPCClient : public QObject {
    Q_OBJECT
public:
    BGMRPCClient (QObject* parent = nullptr);

    void setServerAddress (const QHostAddress& address,
                           quint16 port);
    void setUrl (const QUrl& url);
    void setUrl (const QString& url, int port);
    void setProxy (const QNetworkProxy& proxy);

    QVariant callMethod (const QString& obj, const QString& method,
                         const QJsonArray& args);
    QAbstractSocket::SocketState state() const;
    //QJsonDocument readReturn ();

signals:
    void remoteSignal (const QString& obj, const QString& signal,
                       const QJsonArray& args);
    void connected ();
    void disconnected ();
    void stateChanged (QAbstractSocket::SocketState state);

public slots:
    void connectToHost ();
    void close ();

private:
    QWebSocket Socket;
    //bool ReturnBuffReady;
    //QJsonDocument ReturnBuff;
    //QEventLoop WaitReadLoop;
    QMap < QString, Calling* > Callings;
    QEventLoop WaitConnectLoop;
    int MID;

    QUrl Url;
    QNetworkProxy Proxy;
};

#endif // BGMRPCCLIENT_H
