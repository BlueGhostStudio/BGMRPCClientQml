#ifndef CLIENT_H
#define CLIENT_H

#include <QLocalSocket>
#include <QObject>
#include <QWebSocket>

namespace NS_BGMRPC
{

class BGMRPC;

class Client : public QObject
{
    Q_OBJECT
public:
    explicit Client(BGMRPC* bgmrpc, QWebSocket* socket,
                    QObject* parent = nullptr);
    ~Client();

    bool operator==(const Client* other) const;
    bool operator==(quint64 cliID) const;

    //    quint64 ID() const;

signals:

private: ///< private methods
    QLocalSocket* connectObject(const QString& mID, const QString& objName);
    QLocalSocket* relatedObjectSocket(const QString& objName) const;

    bool requestCall(const QByteArray& data);
    void returnData(const QByteArray& data);
    //    void returnError(quint8 errNO, const QString& errStr);

private:
    BGMRPC* m_BGMRPC;
    QWebSocket* m_BGMRPCSocket;
    QMap<QString, QLocalSocket*> m_relatedObjectSockets;
    quint64 m_ID;

    static quint64 m_totalID;
};
} // namespace NS_BGMRPC
#endif // CLIENT_H
