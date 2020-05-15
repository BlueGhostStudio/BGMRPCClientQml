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

    //! \name 初始化和对象属性
    //! @{
    /*!
     * \brief 注册对象
     * \param 对象明
     * \return 返回成功与否
     */
    bool registerObject(const QByteArray& name);
    /*!
     * \brief 获取当前对象名
     * \return 当前对象名
     */
    QString objectName() const;
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
    QPointer<Caller>
    findRelatedCaller(std::function<bool(QPointer<Caller>)> callback);
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
    QVariant callLocalMethod(QPointer<Caller> caller, const QString& object,
                             const QString& method, const QVariantList& args);
    /*! \overload */
    QVariant callLocalMethod(const QString& object, const QString& method,
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
    void objectDisconnected();
    void callerExisted(QPointer<Caller>);
    void relatedCallerExited(QPointer<Caller>);

private slots:
    //    void callMethod();
    void newCaller();

protected:
    virtual bool verification(QPointer<Caller> caller, const QString& method,
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
