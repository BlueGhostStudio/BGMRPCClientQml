#ifndef _BGMRPC_H
#define _BGMRPC_H

#include <QtCore>
//#include <QtQuick/QQuickItem>
#include <QtQuick>
#include <socket.h>
#include <bgmrpcsocketbuffer.h>

class proc;
class _RStep;
extern QQmlEngine* engine;

class BGMRPC : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(int port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QQmlListProperty < proc > procs READ procs)
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY connectStateChanged)
    Q_PROPERTY(bool isDirectSocket READ isDirectSocket NOTIFY isDirectSocketChanged)

public:
    explicit BGMRPC(QQuickItem *parent = 0);

    void setHost (const QString& h);
    QString host () const;
    void setPort (int p);
    int port () const;

    Q_INVOKABLE void connectToHost (const QString& host, quint16 port);
    Q_INVOKABLE void connectToHost ();
    bool isConnected () const;
    bool isDirectSocket () const;
    void close();

    Q_INVOKABLE BGMRPC* newInstance () const;
    QQmlListProperty < proc > procs ();
    Q_INVOKABLE proc* newProc ();

    void call (_RStep* aStep, const QString& object,
               const QString& method, const QJSValue&args);

    Q_INVOKABLE void sendRawData (const QByteArray& data);

signals:
    void hostChanged ();
    void portChanged ();

    void connectStateChanged ();
    void connected ();
    void disconnected ();
    void isDirectSocketChanged ();
    void error (QAbstractSocket::SocketError err, const QString& errorString);
    void message (const QString& object, const QString& signal, const QJSValue& values);

    void dataReviced (const QByteArray& data);
    void dataWriten (qint64 size);
    void done (const QJSValue& rets);

private slots:
    void onDataRevice ();
    void onError (QAbstractSocket::SocketError err);

private:
    QString Host;
    quint16 Port;
    __socket* CliSocket;
    BGMRPCSocketBuffer SocketBuffer;

    quint16 CSID;
    QMap < QString, _RStep* > CallingSteps;
    bool DirectSocket;

    static void appendProc (QQmlListProperty < proc >* list, proc* p);
    QJsonValue toJson (const QJSValue& jsv);
    QJSValue toJsValue (const QJsonValue& json);
};

#endif // _BGMRPC_H
