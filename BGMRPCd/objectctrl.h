#ifndef OBJECTCTRL_H
#define OBJECTCTRL_H

#include <bgmrpccommon.h>

#include <QLocalSocket>
#include <QObject>

namespace NS_BGMRPC {

class Client;
class BGMRPC;
// class ObjectInterface;

class ObjectCtrl : public QObject {
    Q_OBJECT
public:
    explicit ObjectCtrl(/*BGMRPC* bgmrpc, */ QLocalSocket* socket,
                        QObject* parent = nullptr);

    QString dataSocketName() const;
    void sendCtrlData(const QByteArray& data);

signals:
    void registerObject(const QString&);
    void checkObject(const QString&);
    void getConfig(quint8 cnf);
    void getSetting(const QByteArray& key);
    void removeObject(const QString&);
    void stopServer();
    void detachObject(const QString&);
    void listObjects();
    //    void createObject(const QString&, const QString&, const QString&);

public slots:

private:
    //    BGMRPC* m_BGMRPC;
    QString m_objectName;
    QString m_dataSocketName;
    QLocalSocket* m_ctrlStroke;
    bool m_daemonCtrl;
    //    QList < Client* > m_relatedClients;

    friend BGMRPC;
};

}  // namespace NS_BGMRPC
#endif  // OBJECTCTRL_H
