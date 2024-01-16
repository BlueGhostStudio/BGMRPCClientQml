#ifndef OBJECTINTERFACE_H
#define OBJECTINTERFACE_H

#include <QLocalServer>
#include <QLocalSocket>
#include <QMap>
#include <QMutex>
#include <QObject>
#include <functional>

#include "ObjectInterface_global.h"
#include "mthdAdaptIF.h"

namespace NS_BGMRPCObjectInterface {

class Caller;
using T_METHOD = std::function<QVariant(QPointer<Caller>, const QVariantList&)>;

class OBJECTINTERFACE_EXPORT ObjectInterface : public QObject {
    Q_OBJECT
public:
    explicit ObjectInterface(QObject* parent = nullptr);
    ~ObjectInterface();

    void setAppPath(const QString& path);
    QString appPath() const;

    void setDataPath(const QString& path);
    QString dataPath() const;

    QString objectName() const;
    QString objectID() const;
    QString group() const;
    QString appName() const;

    QVariant interface(QPointer<Caller>, bool readable);

    bool setup(const QByteArray& appName, const QByteArray& name,
               const QByteArray& grp = QByteArray(), int argc = 0,
               char** argv = nullptr, bool noAppPrefix = false);

    QVariant call(bool block, QPointer<Caller> caller, const QString& obj,
                  const QString& method, const QVariantList& args);
    QVariant call(QPointer<Caller> caller, const QString& obj,
                  const QString& method, const QVariantList& args);
    QVariant call(const QString& obj, const QString& method,
                  const QVariantList& args);
    void callNonblock(const QString& obj, const QString& method,
                      const QVariantList& args);

    void addRelatedCaller(QPointer<Caller> caller);
    bool removeRelatedCaller(QPointer<Caller> caller);
    QPointer<Caller> findRelatedCaller(
        std::function<bool(QPointer<Caller>)> callback);

    void emitSignal(const QString& signal, const QVariant& args);

    void setPrivateData(QPointer<Caller> caller, const QString& name,
                        const QVariant& data);
    QVariant privateData(QPointer<Caller> caller, const QString& name);

public slots:
    void detachObject();

private slots:
    void newCaller();

signals:
    void callerExited(QPointer<Caller>);
    void relatedCallerExited(QPointer<Caller>);

protected:
    virtual bool initial(int, char**);
    virtual bool verification(QPointer<Caller>, const QString&,
                              const QVariantList&);
    virtual void exec(const QString& mID, QPointer<Caller> caller,
                      const QString& method, const QVariantList& args);
    virtual void registerMethods() = 0;

    /*
     *  RM: Register Method
     * RMV: RMV: Register Method with Variable arguments. "V" signifies Variable
     * parameters stored in the argument list.
     */
    template <typename T, typename... Args, typename First, typename... Rest>
    void RM(const QString& method, const QString& desc,
            QVariant (T::*funPtr)(Args...), const First& first,
            const Rest&... rest) {
        QStringList params;
        genParamInfo(params, first, rest...);
        m_IFDictIndex.append(method);
        m_IFDict[method] = { method + "(" + params.join(", ") + ")", desc };
        m_methods[method] =
            AdapIF(static_cast<T*>(this), funPtr, first, rest...);
    }
    template <typename T, typename... Args>
    void RM(const QString& method, const QString& desc,
            QVariant (T::*funPtr)(Args...)) {
        m_IFDictIndex.append(method);
        m_IFDict[method] = { method + "()", desc };
        m_methods[method] = AdapIF(static_cast<T*>(this), funPtr);
    }
    template <typename T>
    void RMV(const QString& methodName, const QString& desc,
             QVariant (T::*memberMethod)(const QPointer<Caller>,
                                         const QVariantList&)) {
        m_IFDictIndex.append(methodName);
        m_IFDict[methodName] = { methodName + "(arg0, arg1, ...)", desc };
        m_methods[methodName] =
            std::bind(memberMethod, static_cast<T*>(this),
                      std::placeholders::_1, std::placeholders::_2);
    }

protected:
    QString m_appPath;
    QString m_dataPath;
    QString m_name;
    QString m_ID;
    QString m_appName;
    QString m_grp;

    QMutex m_objMutex;

    QHash<QString, T_METHOD> m_methods;
    QHash<QString, QStringList> m_IFDict;
    QStringList m_IFDictIndex;

    QHash<quint64, QPointer<Caller>> m_relatedCaller;
    QHash<quint64, QVariantMap> m_privateDatas;

    QLocalServer* m_dataServer = nullptr;
    QLocalSocket* m_objectConnecter = nullptr;
};

}  // namespace NS_BGMRPCObjectInterface
#endif  // OBJECTINTERFACE_H
