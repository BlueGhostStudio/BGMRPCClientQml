#ifndef CALLER_H
#define CALLER_H

#include <bgmrpccommon.h>

#include <QLocalSocket>
#include <QObject>
namespace NS_BGMRPCObjectInterface {
class ObjectInterface;
class Caller : public QObject {
    Q_OBJECT
public:
    explicit Caller(ObjectInterface* callee, QLocalSocket* socket,
                    QObject* parent = nullptr);
    ~Caller();

    qint64 ID() const;
    bool exited() const;
    bool isInternalCall() const;
    QString app() const;
    QString object() const;
    QString grp() const;
    //    void setID(quint64 id);

signals:
    void clientExited();
    void returnData(const QString& mID, const QVariant& data);
    void emitSignal(const QString& signal, const QVariant& args);
    void returnError(const QString& mID, quint8 errNo, const QString& errStr);

private slots:
    void onReturnData(const QString& mID, const QVariant& data);
    void onEmitSignal(const QString& signal, const QVariant& args);
    void onReturnError(const QString& mID, quint8 errNo, const QString& errStr);

private:
    void unsetDataSocket();

private:
    QLocalSocket* m_dataSocket;
    qint64 m_ID;
    //    bool m_localCall;
    bool m_exited;
    ObjectInterface* m_callee;
    QString m_calleeMethod;
    QString m_callerApp;
    QString m_callerObject;
    QString m_callerGrp;
    NS_BGMRPC::Call m_callType;
    //    static quint64 m_totalID;

    friend ObjectInterface;
};
}  // namespace NS_BGMRPCObjectInterface
#endif  // CALLER_H
