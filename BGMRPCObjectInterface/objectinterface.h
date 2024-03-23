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
using T_METHODPTR =
    std::function<QVariant(QPointer<Caller>, const QVariantList&)>;

struct t_method {
    QString m_decl;
    QString m_desc;
    bool m_isAsync;
    T_METHODPTR m_methodPtr;

    t_method() = default;
    t_method(const QString& desc, bool async = false);
};

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

    QByteArray objCtrlCmd(quint8 cmd, const QByteArray& arg);

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
    bool containsRelatedCall(QPointer<Caller> caller);
    QPointer<Caller> findRelatedCaller(
        std::function<bool(QPointer<Caller>)> callback);

    void emitSignal(const QString& signal, const QVariant& args);
    void asyncReturn(QPointer<Caller> caller, const QVariantMap& callInfo,
                     const QVariant& retData);
    void asyncReturnError(QPointer<Caller> caller, const QVariantMap& callInfo,
                    quint8 errNo, const QString& errStr);

    void setPrivateData(QPointer<Caller> caller, const QString& name,
                        const QVariant& data);
    QVariant privateData(QPointer<Caller> caller, const QString& name);

public slots:
    void detachObject();
    void objCtrlCmdWork(quint8 cmd, const QByteArray& arg);

private slots:
    void newCaller();

signals:
    void callerExited(QPointer<Caller>);
    void relatedCallerExited(QPointer<Caller>);
    void objCtrlCmdReady(const QByteArray& data);

protected:
    virtual bool initial(int, char**);
    virtual bool verification(QPointer<Caller>, const QString&,
                              const QVariantList&);
    virtual void exec(const QString& mID, QPointer<Caller> caller,
                      const QString& method, const QVariantList& args);
    virtual void registerMethods() = 0;

    /*
     * RM: Register Method
     * RMV: Register Method with Variable arguments. "V" signifies Variable
     * parameters stored in the argument list.
     */
    template <typename T, typename... Args, typename First, typename... Rest>
    void RM(const QString& name, t_method method /*const QString& desc*/,
            QVariant (T::*funPtr)(Args...), const First& first,
            const Rest&... rest) {
        QStringList params;
        genParamInfo(params, first, rest...);

        method.m_decl = name + "(" +
                        (method.m_isAsync ? params.mid(1) : params).join(", ") +
                        (method.m_isAsync ? ") async" : ")");
        method.m_methodPtr =
            AdapIF(static_cast<T*>(this), funPtr, first, rest...);
        m_methods[name] = method;
    }
    template <typename T, typename... Args>
    void RM(const QString& name, t_method method,
            QVariant (T::*funPtr)(Args...)) {
        // m_IFDict[method] = { method + "()", desc };
        method.m_decl = name + "()";
        method.m_methodPtr = AdapIF(static_cast<T*>(this), funPtr);
        m_methods[name] = method;
    }
    template <typename T>
    void RMV(const QString& name, t_method method,
             QVariant (T::*memberMethod)(const QPointer<Caller>,
                                         const QVariantList&)) {
        // m_IFDict[name] = { name + "(arg0, arg1, ...)", desc };
        method.m_decl = name + "(arg0, arg1, ...)";
        method.m_methodPtr =
            std::bind(memberMethod, static_cast<T*>(this),
                      std::placeholders::_1, std::placeholders::_2);
        m_methods[name] = method;
    }

protected:
    QString m_appPath;
    QString m_dataPath;
    QString m_name;
    QString m_ID;
    QString m_appName;
    QString m_grp;

    QMutex m_objMutex;

    QHash<QString, t_method> m_methods;
    // QHash<QString, QStringList> m_IFDict;

    QHash<quint64, QPointer<Caller>> m_relatedCaller;
    QHash<quint64, QVariantMap> m_privateDatas;

    QLocalServer* m_dataServer = nullptr;
    QLocalSocket* m_objectConnecter = nullptr;
};

}  // namespace NS_BGMRPCObjectInterface
#endif  // OBJECTINTERFACE_H
