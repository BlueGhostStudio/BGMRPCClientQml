#ifndef BGMRPCC_H
#define BGMRPCC_H

#include <QtCore>
#include <QtQuick>
#include "socket.h"

class BGMRPCC : public QObject
{
    Q_PROPERTY(QJSValue ONOpened READ openedFun WRITE setOpenedFun)
    Q_PROPERTY(QJSValue ONClosed READ closedFun WRITE setClosedFun)
    Q_PROPERTY(QJSValue ONError READ errorFun  WRITE setErrorFun)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectStateChanged)

    Q_OBJECT
public:
    explicit BGMRPCC(QJSEngine* e, QObject *parent = 0);

    Q_INVOKABLE void testRet ();

    Q_INVOKABLE QJSValue newMethod (const QString& obj,
                                    const QString& met,
                                    const QJSValue& args);
    Q_INVOKABLE QJSValue newLMethod (const QJSValue& cb);
    Q_INVOKABLE void call(const QJSValue& m);
    Q_INVOKABLE void lcall (const QJSValue& m);

    Q_INVOKABLE void setHost (const QString& host);
    Q_INVOKABLE void setPort (quint16 port);
    Q_INVOKABLE void connectToHost (const QString& host,
                                   quint16 port);
    Q_INVOKABLE void connectToHost ();
    Q_INVOKABLE void close ();
    Q_INVOKABLE void setProxy(const QNetworkProxy& proxy);

    Q_INVOKABLE void setMessageCallback (const QJSValue& cb);
    Q_INVOKABLE void setMessageCallback (const QString& obj,
                                         const QJSValue& cb);
    Q_INVOKABLE void setMessageCallback (const QString& obj,
                                         const QString& sig,
                                         const QJSValue cb);

    void setOpenedFun (const QJSValue& jsv);
    QJSValue openedFun () const;
    void setClosedFun (const QJSValue& jsv);
    QJSValue closedFun () const;
    void setErrorFun (const QJSValue& jsv);
    QJSValue errorFun () const;

    Q_INVOKABLE bool isConnected ();


signals:
    void connectStateChanged ();

public slots:
    void onConnected ();
    void onClosed ();
    void onError (QAbstractSocket::SocketError);
    void dataRevice ();

private:
    QJSEngine* Engine;
    QJSValue RPCThis;
    QMap < QString, QJSValue > CallingMethods;
    QJSValue OpenedFun;
    QJSValue ClosedFun;
    QJSValue ErrorFun;

    QMap < QString, QJSValue > MessageCallbacks;

    QJSValue hw_js;

    int MID;
    QString Host;
    quint16 Port;
    QNetworkProxy Proxy;
    __socket* CliSocket;

    QJsonValue toJson (const QJSValue& jsv);
    QJSValue toJsValue (const QJsonValue& json);
};

#endif // BGMRPCC_H
