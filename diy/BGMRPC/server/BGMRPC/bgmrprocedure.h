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
extern QMutex mutex;

class BGMRPC;
class BGMRObjectInterface;
class BGMRProcedure;

class callThread : public QThread {
    Q_OBJECT
public:
    explicit callThread (const QString& mID, BGMRProcedure* p, BGMRObjectInterface* o,
                         const QString& m, const QJsonArray& as,
                         QObject* parent = 0);

protected:
    void run ();

    QString MID;
    BGMRProcedure* OwnProc;
    BGMRObjectInterface* Object;
    QString Method;
    QJsonArray Args;
};

class BGMRProcedure : public QObject
{
    Q_OBJECT
public:
    explicit BGMRProcedure(BGMRPC* r,
                           __socket* socket,
                           QObject *parent = 0);
    ~BGMRProcedure ();

//    QJsonValueRef privateData (const QString& key);
//    QJsonValue privateData (const QString& key) const;

    QJsonValueRef privateData (const BGMRObjectInterface* obj,
                               const QString& key);
    QJsonValue privateData (const BGMRObjectInterface* obj,
                            const QString& key) const;


    qulonglong pID () const;
    void close ();
    void setObject (BGMRObjectInterface* object);
//    bool isKeepConnected () const;
    __socket* procSocket() const;
    __socket* switchDirectSocket();
    bool isDirectSocket () const;
    BGMRPCSocketBuffer* socketBuffer (); //
    void switchProcedure ();
    QJsonArray callMethod (const QString& obj, const QString& method,
                           const QJsonArray& args);

signals:
    void procExited (qulonglong id);
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
    void callMethod ();

private:
    // private member method

    QMap < QString, QJsonObject > PrivateData;
    // private data
    BGMRPC* RPC;
    BGMRObjectInterface* Object;
    __socket* ProcSocket;
    BGMRPCSocketBuffer SocketBuffer;
    bool DirectSock;

    qulonglong PID;
//    bool KeepConnected;
};

}
#endif // BGMRPROCEDURE_H
