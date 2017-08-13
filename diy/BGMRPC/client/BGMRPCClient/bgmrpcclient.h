#ifndef BGMRPCCLIENT_H
#define BGMRPCCLIENT_H

#include <QtWebSockets>
#include <QHostAddress>
#include <QObject>
#include <QNetworkProxy>
#include <QtCore>

class BGMRPCClient : public QObject {
    Q_OBJECT
public:
    BGMRPCClient (QObject* parent = nullptr);

    void setServerAddress (const QHostAddress& address,
                           quint16 port);
    void setUrl (const QUrl& url);
    void setUrl (const QString& url, int port);
    void setProxy (const QNetworkProxy& proxy);
    void connectToHost();

    QVariant callMethod(const QString& obj, const QString& method,
                             const QJsonArray& args);
    QJsonDocument readReturn ();

signals:
    void remoteSignal (const QString& obj, const QString& signal,
                       const QJsonArray& args);
private:
    QWebSocket Socket;
    bool ReturnBuffReady;
    QJsonDocument ReturnBuff;
    QEventLoop WaitReadLoop;
    QEventLoop WaitConnectLoop;
    int MID;

    QUrl Url;
    QNetworkProxy Proxy;
};

#endif // BGMRPCCLIENT_H
