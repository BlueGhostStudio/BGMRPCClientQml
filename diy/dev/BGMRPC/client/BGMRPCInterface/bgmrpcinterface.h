#ifndef BGMRPCINTERFACE_H
#define BGMRPCINTERFACE_H

#include <QHostAddress>
#include "socket.h"
#include <QObject>
#include <QNetworkProxy>

#include "bgmrpcinterface_global.h"
#include "datatype.h"

#define SENDDATA_MAXSIZE 1024

namespace BGMircroRPCInterface {

class BGMRPCINTERFACESHARED_EXPORT cliRawDataTrans : public QObject
{
    Q_OBJECT

public:
    cliRawDataTrans (__socket* socket, QObject* parent = 0);

    bool sendRawData (const QByteArray& data);
    QByteArray getRawData (qint16 len = -1);
    void close ();

private slots:
    void readData ();

private:
    __socket* CliRawSocket;
    QByteArray BuffData;
    QEventLoop WaitLoop;
};

class BGMRPCINTERFACESHARED_EXPORT cliDataTrans : public QObject
{
    Q_OBJECT

public:
    cliDataTrans (QObject* parent = 0);
    void setSID (qulonglong id);
    BGMircroRPC::returnedValue_t getReturnedValues ();
    __socket* socket(bool  detach = false);
//    void setRawSocket ();

signals:
    void disconnected (qulonglong id);
//    void remoteSignal (const BGMircroRPC::returnedValue_t& sig);
    void remoteSignal (const QString& obj, const QString& signal,
                       const QJsonArray& args);

public slots:
    void onDisconnected ();
    void readRemoteData ();

private:
    qulonglong SID;
    bool BufferReady;
    bool WaitDelete;

    BGMircroRPC::returnedValue_t Buffer;
    QEventLoop WaitReadLoop;
    __socket* CliSocket;
};

class BGMRPCINTERFACESHARED_EXPORT BGMRPCInterface : public QObject
{
    Q_OBJECT

public:
    BGMRPCInterface(QObject* parent = 0);
    BGMRPCInterface(const QHostAddress& address,
                    quint16 port,
                    QObject *parent = 0);

    void setServerAddress (const QHostAddress& address,
                           quint16 port);
    void setHost (const QString& host);
    void setProxy (const QNetworkProxy& proxy);
    BGMircroRPC::returnedValue_t
    callMethod (const QString& obj,
                const QString& method,
                const QJsonArray& args,
                bool keepConnected = false);
    BGMircroRPC::returnedValue_t
    callMethod (qulonglong id,
                const QString& method,
                const QJsonArray& args);
    BGMircroRPC::returnedValue_t
    callMethod (qulonglong id, const QString& object,
                const QString& method, const QJsonArray& args);

    cliRawDataTrans* callMethodRaw(const QString& obj, const QString& method,
                              const QJsonArray& args);
    cliRawDataTrans* callMethodRaw(qulonglong id, const QString& method,
                              const QJsonArray& args);
    cliRawDataTrans* callMethodRaw(qulonglong id, const QString& object,
                              const QString& method, const QJsonArray& args);

signals:
    void remoteSignal (const QString& object, const QString& signal,
                       const QJsonArray& args);

public slots:
    void removeCliSocket (qulonglong id);

private:
    bool connectToHost (__socket* socket);

    QHostAddress ServerAddress;
    quint16 ServerPort;
    QString Hosts;
    QNetworkProxy Proxy;

    QMap < qulonglong, cliDataTrans* > CliDataTrans;
};

}

#endif // BGMRPCINTERFACE_H
