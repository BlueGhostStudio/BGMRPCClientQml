#ifndef CALLER_H
#define CALLER_H

#include <QLocalSocket>
#include <QObject>
namespace NS_BGMRPCObjectInterface {
class ObjectInterface;
class Caller : public QObject {
    Q_OBJECT
public:
    explicit Caller(ObjectInterface* callee, QLocalSocket* socket,
                    QObject* parent = nullptr);

    qint64 ID() const;
    //    void setID(quint64 id);
    void returnData(const QString& mID, const QVariant& data);
    void emitSignal(const QString& signal, const QVariant& args);
    void returnError(quint8 errNo, const QString& errStr);

signals:
    void clientExited();

private:
    QLocalSocket* m_dataSocket;
    qint64 m_ID;
    bool m_localCall;
    ObjectInterface* m_callee;
    //    static quint64 m_totalID;

    friend ObjectInterface;
};
} // namespace NS_BGMRPCObjectInterface
#endif // CALLER_H
