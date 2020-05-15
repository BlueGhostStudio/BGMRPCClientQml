#ifndef CALLER_H
#define CALLER_H

#include <QLocalSocket>
#include <QObject>
namespace NS_BGMRPCObjectInterface
{
class ObjectInterface;
class Caller : public QObject
{
    Q_OBJECT
public:
    explicit Caller(ObjectInterface* callee, QLocalSocket* socket,
                    QObject* parent = nullptr);
    ~Caller();

    qint64 ID() const;
    bool exited() const;
    //    void setID(quint64 id);

signals:
    void clientExited();
    void returnDataReady(const QString& mID, const QVariant& data);
    void emitSignalReady(const QString& signal, const QVariant& args);
    void returnErrorReady(const QString& mID, quint8 errNo,
                          const QString& errStr);

private slots:
    void returnData(const QString& mID, const QVariant& data);
    void emitSignal(const QString& signal, const QVariant& args);
    void returnError(const QString& mID, quint8 errNo, const QString& errStr);

private:
    QLocalSocket* m_dataSocket;
    qint64 m_ID;
    bool m_localCall;
    bool m_exited;
    ObjectInterface* m_callee;
    //    static quint64 m_totalID;

    friend ObjectInterface;
};
} // namespace NS_BGMRPCObjectInterface
#endif // CALLER_H
