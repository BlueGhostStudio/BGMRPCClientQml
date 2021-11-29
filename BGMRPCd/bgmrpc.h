#ifndef BGMRPC_H
#define BGMRPC_H

#include <QLocalServer>
#include <QObject>
#include <QSettings>
#include <QWebSocket>
#include <QWebSocketServer>

namespace NS_BGMRPC {

class CtrlBase;
class ServerCtrl;
class ObjectPlug;

class BGMRPC : public QObject {
    Q_OBJECT
public:
    explicit BGMRPC(QObject* parent = nullptr);
    ~BGMRPC();

    bool start(/*ip, port*/);
    void setAddress(const QHostAddress& address);
    void setPort(quint16 port);
    ObjectPlug* objectCtrl(const QString& name);

    void initial(const QString& file);

public slots:
    /*void ctrl_registerObject(const QString& name);
    void ctrl_checkObject(const QString& name);
    void ctrl_getConfig(quint8 cnf);
    void ctrl_getSetting(const QByteArray& key);
    void ctrl_detachObject(const QString& name);
    void ctrl_listObjects();*/
    void newClient();

private:
    QByteArray listObjects();
    void stopServer();
    bool detachObject(const QByteArray& name);
    void removeObject(const QByteArray& name);
    bool registerObject(ObjectPlug* ctrl, const QByteArray& name);
    bool checkObject(const QByteArray& name);
    QByteArray getConfig(quint8 cnf);
    QByteArray getSetting(const QByteArray& key);

private:
    QSettings* m_settings;

    QLocalServer* m_serverCtrlServer;
    // QLocalServer* m_objectCtrlServer;
    QLocalServer* m_objectSocketServer;
    QMap<QString, ObjectPlug*> m_objects;

    QWebSocketServer* m_BGMRPCServer;
    QHostAddress m_address;
    quint16 m_port;

    friend CtrlBase;
    friend ServerCtrl;
    friend ObjectPlug;
};

}  // namespace NS_BGMRPC
#endif  // BGMRPC_H
