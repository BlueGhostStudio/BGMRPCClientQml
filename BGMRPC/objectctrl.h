#ifndef OBJECTCTRL_H
#define OBJECTCTRL_H

#include <QLocalSocket>
#include <QObject>

namespace NS_BGMRPC {

class Client;
class BGMRPC;
// class ObjectInterface;

class ObjectCtrl : public QObject {
    Q_OBJECT
public:
    explicit ObjectCtrl(BGMRPC* bgmrpc, QLocalSocket* socket,
                        QObject* parent = nullptr);

    QString dataSocketName() const;
    //    void callMethod(Client* cli, const QByteArray& data);

signals:
    void registerObject(const QString&);
    void removeObject(const QString&);

public slots:

private:
    BGMRPC* m_BGMRPC;
    QString m_objectName;
    QString m_dataSocketName;
    QLocalSocket* m_ctrlStroke;
    //    QList < Client* > m_relatedClients;
};

} // namespace NS_BGMRPC
#endif // OBJECTCTRL_H
