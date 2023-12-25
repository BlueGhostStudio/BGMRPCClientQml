#ifndef OBJECTINTERFACE_H
#define OBJECTINTERFACE_H

#include <caller.h>

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QLocalServer>
#include <QLocalSocket>
#include <QObject>
#include <QPointer>
#include <functional>

#include "ObjectInterface_global.h"

#define REG_METHOD(methodName, memberMethod) \
    m_methods[methodName] = std::bind(       \
        memberMethod, this, std::placeholders::_1, std::placeholders::_1);

namespace NS_BGMRPCObjectInterface {

class Caller;
class ObjectInterface;

/*typedef QVariant (*T_METHOD)(ObjectInterface*, QPointer<Caller>,
                             const QVariantList&);*/
using T_METHOD = std::function<QVariant(QPointer<Caller>, const QVariantList&)>;

class OBJECTINTERFACE_EXPORT ObjectInterface : public QObject {
    Q_OBJECT
public:
    ObjectInterface(QObject* parent = nullptr);

    void setAppPath(const QByteArray& path);
    QByteArray appPath() const;
    void setDataPath(const QByteArray& path);
    QByteArray dataPath() const;

    //! \name 初始化和对象属性
    //! @{
    /*!
     * \brief 注册对象
     * \param 对象明
     * \return 返回成功与否
     */
    /*bool registerObject(const QByteArray& appName, const QByteArray& name,
                        const QByteArray& grp = QByteArray());*/
    /*static QLocalSocket* plugIntoBGMRPC(const QByteArray& group,
                                        const QByteArray& app,
                                        const QByteArray& name);*/

    bool setup(const QByteArray& appName, const QByteArray& name,
               const QByteArray& grp = QByteArray(), int argc = 0,
               char** argv = nullptr, bool noAppPrefix = false);
    /*!
     * \brief 获取当前对象名
     * \return 当前对象名
     */
    QString objectName() const;
    QString objectID() const;
    QString group() const;
    QString appName() const;
    //! @}

    //! \name 关联调用者(客户端)
    //! @{
    /*!
     * \brief 增加关联调用者
     * \param 调用者
     */
    void addRelatedCaller(QPointer<Caller> caller);
    /*!
     * \brief 删除关联调用者
     * \param 调用者
     * \return
     */
    bool removeRelatedCaller(QPointer<Caller> caller);
    /*!
     * \brief 查找当前调用者
     * \param 回调函数，若查找成功此回调函数返回 true
     * \return 返回查找到的关联调用者，若无调用者则返回空
     */
    QPointer<Caller> findRelatedCaller(
        std::function<bool(QPointer<Caller>)> callback);
    //! @}

    //! \name 向关联调用者广播信号
    //! @{
    /*!
     * \brief emitSignal
     * \param signal
     * \param args
     */
    void emitSignal(const QString& signal, const QVariant& args);
    //! @}

    //! \name 调用远端内部其他对象方法
    //! @{
    /*!
     * \brief callLocalMethod
     * \param caller
     * \param object
     * \param method
     * \param args
     * \return
     */
    QVariant call(QPointer<Caller> caller, const QString& object,
                  const QString& method, const QVariantList& args);
    /*! \overload */
    QVariant call(const QString& object, const QString& method,
                  const QVariantList& args);
    /*void callLocalMethodNonblock(QPointer<Caller> caller, const QString&
       object, const QString& method, const QVariantList& args);*/
    void callNonblock(const QString& object, const QString& method,
                      const QVariantList& args);
    //! @}

    //! \name 调用者的私有对象数据
    //! @{
    /*!
     * \brief setPrivateData
     * \param caller
     * \param name
     * \param data
     */
    void setPrivateData(QPointer<Caller> caller, const QString& name,
                        const QVariant& data);
    /*!
     * \brief privateData
     * \param caller
     * \param name
     * \return
     */
    QVariant privateData(QPointer<Caller> caller, const QString& name) const;
    //! @}

signals:
    //    void objectDisconnected();
    void callerExisted(QPointer<NS_BGMRPCObjectInterface::Caller>);
    void relatedCallerExited(QPointer<NS_BGMRPCObjectInterface::Caller>);

    void thread_signal_call(bool block, qint64 callerID, const QString& object,
                            const QString& method, const QVariantList& args);
    void thread_signal_return(const QVariant&);

public slots:
    void detachObject();

private slots:
    //    void callMethod();
    void newCaller();
    void on_thread_call(bool block, qint64 callerID, const QString& object,
                        const QString& method, const QVariantList& args);

protected:
    virtual void initial(int, char**);
    virtual bool verification(QPointer<Caller> caller, const QString& method,
                              const QVariantList& args);
    virtual void exec(const QString& mID, QPointer<Caller> caller,
                      const QString& methodName, const QVariantList& args);
    virtual void registerMethods() = 0;

protected:
    QLocalSocket* m_objectPlug;
    QLocalServer* m_dataServer;
    QByteArray m_appPath;
    QByteArray m_dataPath;
    QByteArray m_name;
    QByteArray m_ID;
    QByteArray m_appName;
    QByteArray m_grp;
    //    QMap<QString, T_METHOD> m_methods;

    /*QMap<QString, std::function<QVariant(ObjectInterface*, QPointer<Caller>,
                                         const QVariantList&)>>
        m_methods;*/
    QMap<QString, T_METHOD> m_methods;
    QMap<quint64, QPointer<Caller>> m_relatedCaller;

    typedef QVariantMap t_priData;
    QMap<quint64, t_priData> m_privateDatas;
};

QByteArray refObjName(const QByteArray& grp, const QByteArray& app,
                      const QByteArray& name, bool noAppPrefix = false);
}  // namespace NS_BGMRPCObjectInterface

#endif  // OBJECTINTERFACE_H
