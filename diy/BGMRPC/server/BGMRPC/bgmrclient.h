#ifndef BGMRPROCEDURE_H
#define BGMRPROCEDURE_H

#include <QObject>
#include "bgmrpc_global.h"
#include <socket.h>
#include <bgmrpcsocketbuffer.h>
#include <QJsonObject>
#include <QJsonValue>
#include <QThread>


namespace BGMircroRPCServer {

void defaultPDDeleter (void* data);
template < typename T >
void PDDeleter (void* data) {
    delete (T*)data;
}

extern QMutex mutex;

class BGMRPC;
class BGMRObjectInterface;
class BGMRClient;

class callThread : public QThread {
    Q_OBJECT
public:
    explicit callThread (const QString& mID, BGMRClient* cli, BGMRObjectInterface* o,
                         const QString& m, const QJsonArray& as,
                         QObject* parent = 0);

protected:
    void run ();

    QString MID;
    BGMRClient* OwnCli;
    BGMRObjectInterface* Object;
    QString Method;
    QJsonArray Args;
};

class BGMRClient : public QObject
{
    Q_OBJECT
public:
    explicit BGMRClient(BGMRPC* r,
                        __socket* socket,
                        QObject *parent = 0);
    ~BGMRClient ();

    //    QJsonValueRef privateData (const QString& key);
    //    QJsonValue privateData (const QString& key) const;

    //    QJsonValueRef privateData (const BGMRObjectInterface* obj,
    //                               const QString& key);
    //    QJsonValue privateData (const BGMRObjectInterface* obj,
    //                            const QString& key) const;
    void setPrivateData (const BGMRObjectInterface* obj,
                         const QString& name,
                         void* data, void (*del)(void*));
    void* privateData(const BGMRObjectInterface* obj,
                      const QString& name);
    QJsonValue privateDataJson (const BGMRObjectInterface* obj, const QString& name);
    void setPrivateDataJson (const BGMRObjectInterface* obj, const QString& name, const QJsonValue& value);

    qulonglong cliID () const;
    void close ();
    void setObject (BGMRObjectInterface* object);
    //    bool isKeepConnected () const;
    __socket* clientSocket() const;
    __socket* switchDirectSocket();
    bool isDirectSocket () const;
    BGMRPCSocketBuffer* socketBuffer (); //
    void switchClient ();
    /*QJsonArray callMethod (BGMRObjectInterface* obj, const QString& method,
                           const QJsonArray& args);*/

signals:
    void clientExited (qulonglong id);
    void emitSignal (const BGMRObjectInterface* obj, const QString& signal,
                     const QJsonArray& args);
    void returnValues (const QJsonArray& values, bool directSocketReturn,
                       const QString& mID = QString ());

public slots:
    void onClientSocketDisconnected ();
    void onReturnValues(const QJsonArray& values, bool directSocketReturn,
                        const QString& mID);
    void onEmitSignal (const BGMRObjectInterface* obj, const QString& signal,
                       const QJsonArray& args);

private slots:
    void handleCallRequest ();

private:
    // private member method

    //    QMap < QString, QJsonObject > PrivateData;
    typedef QPair < void*, void (*)(void*) > PriDataType;
    typedef QMap < QString, PriDataType > ObjPriDataType;
    QMap < QString, ObjPriDataType > PrivateData;
    // private data
    BGMRPC* RPC;
    BGMRObjectInterface* Object;
    __socket* ClientSocket;
    BGMRPCSocketBuffer SocketBuffer;
    bool DirectSock;

    qulonglong CliID;
    qulonglong RefThreadCount;
    bool Exited;
    //    bool KeepConnected;
};

}
#endif // BGMRPROCEDURE_H
