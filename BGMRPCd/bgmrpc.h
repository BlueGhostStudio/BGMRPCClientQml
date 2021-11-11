#ifndef BGMRPC_H
#define BGMRPC_H

#include "objectctrl.h"
#include <QLocalServer>
#include <QObject>
#include <QSettings>
#include <QWebSocket>
#include <QWebSocketServer>

namespace NS_BGMRPC
{

class BGMRPC : public QObject
{
    Q_OBJECT
public:
    explicit BGMRPC(QObject* parent = nullptr);
    ~BGMRPC();

    bool start(/*ip, port*/);
    void setAddress(const QHostAddress& address);
    void setPort(quint16 port);
    ObjectCtrl* objectCtrl(const QString& name);

    void initial(const QString& file);

signals:
    void test_addedObject(const QString&);

public slots:
    void ctrl_registerObject(const QString& name);
    void ctrl_checkObject(const QString& name);
    void ctrl_getConfig(quint8 cnf);
    void ctrl_detachObject(const QString& name);
    void ctrl_listObjects();
    void newClient();

private:
    QSettings* m_settings;

    QLocalServer* m_ctrlServer;
    QMap<QString, ObjectCtrl*> m_objects;

    QWebSocketServer* m_BGMRPCServer;
    QHostAddress m_address;
    quint16 m_port;
};

} // namespace NS_BGMRPC
#endif // BGMRPC_H
