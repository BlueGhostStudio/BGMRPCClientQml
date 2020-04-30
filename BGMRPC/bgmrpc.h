#ifndef BGMRPC_H
#define BGMRPC_H

#include "objectctrl.h"
#include <QLocalServer>
#include <QObject>
#include <QWebSocket>
#include <QWebSocketServer>

namespace NS_BGMRPC {

class BGMRPC : public QObject {
    Q_OBJECT
public:
    explicit BGMRPC(QObject* parent = nullptr);
    ~BGMRPC();

    bool start(/*ip, port*/);
    void setAddress(const QHostAddress& address);
    ObjectCtrl* objectCtrl(const QString& name);

signals:
    void test_addedObject(const QString&);

public slots:
    void newObject(const QString& name);
    void newClient();

private:
    QLocalServer* m_ctrlServer;
    QMap<QString, ObjectCtrl*> m_objects;

    QWebSocketServer* m_BGMRPCServer;
    QHostAddress m_address;
    quint16 m_port;

    friend ObjectCtrl;

    //    QLocalServer* test_BGMRPCTcpServer;
};

} // namespace NS_BGMRPC
#endif // BGMRPC_H
