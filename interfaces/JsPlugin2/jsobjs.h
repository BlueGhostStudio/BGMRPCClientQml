#ifndef JSOBJS_H
#define JSOBJS_H

#include <caller.h>
#include <objectinterface.h>

#include <QJSEngine>
#include <QObject>
#include <QPointer>

#include "jsengine.h"

namespace NS_BGMRPCObjectInterface {

class JsCaller : public QObject {
    Q_OBJECT

    Q_PROPERTY(quint64 __ID__ READ cliID)
    Q_PROPERTY(bool __ONLINE__ READ online)
    Q_PROPERTY(bool __ISINTERNALCALL__ READ isInternalCall)
    Q_PROPERTY(QString __APP__ READ app)
    Q_PROPERTY(QString __OBJECT__ READ object)
    Q_PROPERTY(QString __OBJECTID__ READ objectID)
    Q_PROPERTY(QString __GRP__ READ grp)
public:
    explicit JsCaller(QPointer<Caller> caller, QObject* parent = nullptr);

    quint64 cliID() const;
    bool online() const;
    Q_INVOKABLE QJSValue clone() const;
    /*!
     * \brief 设置对象对应调用者的私有数据
     * \deprecated
     * 由于改变了设置私有数据的方式，从原来由客户端设置对应对象的私有数据变为
     * 由对象设置调用者其对应的私有数据；此成员函数已废弃，只为了兼容旧的js脚本。
     * 使用 JsJSObj::setPrivateData
     */
    Q_INVOKABLE void setPrivateData(const QJSValue& obj, const QString& key,
                                    const QJSValue& value);
    /*!
     * \deprecated
     * 由于改变了设置私有数据的方式，从原来由客户端设置对应对象的私有数据变为
     * 由对象设置调用者其对应的私有数据；此成员函数已废弃，只为兼容旧的js脚本。
     * 使用 JsJSObj::privateData
     */
    Q_INVOKABLE QJSValue privateData(const QJSValue& obj, const QString& key);
    /*!
     * \deprecated
     * 已废弃，只为了兼容旧的js脚本，使用 JsCaller::emitSignal(const
     * QString&,const QJSValue&) const
     */
    Q_INVOKABLE void emitSignal(const QJSValue& obj, const QString& signal,
                                const QJSValue& args) const;
    /*!
     * \brief 向调用者发送信号
     * \param 信号
     * \param 信号参数
     */
    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QJSValue& args) const;

    Q_INVOKABLE bool isInternalCall() const;
    Q_INVOKABLE QString app() const;
    Q_INVOKABLE QString object() const;
    Q_INVOKABLE QString objectID() const;
    Q_INVOKABLE QString grp() const;

    QPointer<Caller> caller() const;

signals:

private:
    QPointer<Caller> m_caller;
};

/*!
 * \brief 此JsJSObj类为供Js脚本使用的JsEngine对象的包装
 */
class JsJSObj : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString __NAME__ READ objectName CONSTANT)
    Q_PROPERTY(QString __OBJID__ READ objectID CONSTANT)
    Q_PROPERTY(QString __PATH_APP__ READ appPath CONSTANT)
    Q_PROPERTY(QString __PATH_DATA__ READ dataPath CONSTANT)
    Q_PROPERTY(QString __PATH_MODULES__ READ modulesPath CONSTANT)
    Q_PROPERTY(QString __PWD__ READ PWD CONSTANT)
    Q_PROPERTY(QString __APP__ READ appName CONSTANT)
    Q_PROPERTY(QString __GRP__ READ grp CONSTANT)

public:
    explicit JsJSObj(JsEngine* jsEngine, QObject* parent = nullptr);

    /*!
     * \brief 获取当前对象名称
     */
    QString objectName() const;
    QString objectID() const;
    QString appName() const;
    QString grp() const;

    QString appPath() const;
    QString dataPath() const;
    QString modulesPath() const;
    QString PWD() const;

    JsEngine* jsEngine() const;
    /*!
     * \brief 设置对象对应调用者的私有数据
     * \param caller 调用者
     * \param key 私有数据键名
     * \param value 私有数据的值
     */
    void setPrivateData(const QPointer<Caller>& caller, const QString& key,
                        const QJSValue& value);
    /*!
     * \brief 读取对象对应调用者的私有数据
     * \param caller 调用者
     * \param key 私有数据键名
     */
    QJSValue privateData(QPointer<Caller> caller, const QString& key) const;

    Q_INVOKABLE QJSValue checkObject(const QString& obj);

    /*!
     * \overload
     * \brief 用于脚本的调用的包装
     */
    Q_INVOKABLE void setPrivateData(const QJSValue& caller, const QString& key,
                                    const QJSValue& value);
    /*!
     * \overload
     * \brief 用于脚本的调用的包装
     */
    Q_INVOKABLE QJSValue privateData(const QJSValue& caller,
                                     const QString& key) const;

    Q_INVOKABLE QJSValue call(const QString& object, const QString& method,
                              const QJSValue& args);
    Q_INVOKABLE QJSValue call(const QJSValue& caller, const QString& object,
                              const QString& method, const QJSValue& args);
    Q_INVOKABLE void callNonblock(const QString& object, const QString& method,
                                  const QJSValue& args);

    /*!
     * \brief 将调用者与对象关联
     * \param caller 调用者
     */
    Q_INVOKABLE bool addRelClient(const QJSValue& caller) const;
    /*!
     * \brief 取消调用者与对象的关联
     * \param caller 调用者
     * \return 成功则返回true,否则为false
     */
    Q_INVOKABLE bool removeRelClient(const QJSValue& caller) const;
    /*!
     * \brief 返回与对象关联的客户端
     * \param autoDel 是否自动删除
     * \return 关联的客户端列表
     * \note
     * 如果指定参数autoDel为true，js函数结束后将自动释放内存，如果需要在退出函数保持
     * 使用则指定此参数为false
     */
    Q_INVOKABLE QJSValue relClients(bool autoDel = true) const;
    /*!
     * \brief 获取指定调用者ID的关联客户端
     * \param callerID 调用者ID
     * \param autoDel 是否自动删除，参考 relClients() 的注解
     * \return
     */
    Q_INVOKABLE QJSValue relClient(int callerID, bool autoDel = true) const;
    Q_INVOKABLE bool containsRelClient(const QJSValue& caller) const;
    Q_INVOKABLE QJSValue findRelClient(const QJSValue& callback,
                                       bool autoDel = true) const;

    Q_INVOKABLE void onRelClientRemoved(const QJSValue& handle);

    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QJsonArray& args) const;
    Q_INVOKABLE bool include(const QString& scrFileName) const;
    Q_INVOKABLE void loadModule(const QString& module) const;

    Q_INVOKABLE void collectGarbage();
    Q_INVOKABLE void destroyObject(const QJSValue& obj);

private:
    JsEngine* m_jsEngine;
    QJSValue m_relClientRemovedHandle;
};

}  // namespace NS_BGMRPCObjectInterface
#endif  // JSOBJS_H
