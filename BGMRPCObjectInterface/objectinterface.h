#ifndef OBJECTINTERFACE_H
#define OBJECTINTERFACE_H

#include "ObjectInterface_global.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QPointer>
#include <caller.h>
#include <functional>

#define REG_METHOD(OBJIF, METHOD)                                              \
    [](ObjectInterface* obj, QPointer<Caller> cli,                             \
       const QVariantList& args) -> QVariant {                                 \
        return qobject_cast<OBJIF*>(obj)->METHOD(cli, args);                   \
    }

namespace NS_BGMRPCObjectInterface
{

class Caller;
class ObjectInterface;

typedef QVariant (*T_METHOD)(ObjectInterface*, QPointer<Caller>,
                             const QVariantList&);

class OBJECTINTERFACE_EXPORT ObjectInterface : public QObject
{
    Q_OBJECT
public:
    ObjectInterface(QObject* parent = nullptr);

    bool registerObject(const QByteArray& name);

    QString objectName() const;

    void addRelatedCaller(QPointer<Caller> caller);
    void emitSignal(const QString& signal, const QVariant& args);
    QPointer<Caller>
    findRelatedCaller(std::function<bool(QPointer<Caller>)> callback);

    QVariant callLocalMethod(QPointer<Caller> caller, const QString& object,
                             const QString& method, const QVariantList& args);
    QVariant callLocalMethod(const QString& object, const QString& method,
                             const QVariantList& args);

    void setPrivateData(QPointer<Caller> caller, const QString& name,
                        const QVariant& data);
    QVariant privateData(QPointer<Caller> caller, const QString& name) const;

signals:
    void objectDisconnected();
    void callerExisted(QPointer<Caller>);
    void relatedCallerExisted(QPointer<Caller>);

private slots:
    //    void callMethod();
    void newCaller();

protected:
    virtual bool permit(QPointer<Caller> caller, const QString& method,
                        const QVariantList& args);
    virtual void callMethod(const QString& mID, QPointer<Caller> caller,
                            const QString& methodName,
                            const QVariantList& args);
    virtual void registerMethods() = 0;

protected:
    QLocalSocket* m_ctrlSocket;
    QLocalServer* m_dataServer;
    QString m_name;
    //    QMap<QString, T_METHOD> m_methods;
    QMap<QString, std::function<QVariant(ObjectInterface*, QPointer<Caller>,
                                         const QVariantList&)>>
        m_methods;
    QMap<quint64, QPointer<Caller>> m_relatedCaller;

    typedef QVariantMap t_priData;
    QMap<quint64, t_priData> m_privateDatas;
};
} // namespace NS_BGMRPCObjectInterface

#endif // OBJECTINTERFACE_H
