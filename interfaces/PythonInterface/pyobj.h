#ifndef PYOBJ_H
#define PYOBJ_H

#include <objectinterface.h>

#include <QObject>

#include "pythoninterface.h"

namespace NS_BGMRPCObjectInterface {
class PyCaller : public QObject {
    Q_OBJECT

    Q_PROPERTY(qint64 ID READ ID)
    Q_PROPERTY(bool isLocalCall READ isLocalCall)
    Q_PROPERTY(QString callerObject READ callerObject)
    Q_PROPERTY(QString callerGrp READ callerGrp)

public:
    PyCaller(QPointer<Caller> caller, QObject* parent = nullptr);
    ~PyCaller();

    qint64 ID() const;
    bool isLocalCall() const;
    QString callerObject() const;
    QString callerGrp() const;

    Q_INVOKABLE bool online() const;  // test ✔
    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QVariant& args) const;  // test ✔

    Q_INVOKABLE NS_BGMRPCObjectInterface::PyCaller* clone() const;  // test ✔
    Q_INVOKABLE void destory();                                     // test ✔

    QPointer<Caller> caller() const;

private:
    QPointer<Caller> m_caller;
};

class PyObj : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString __NAME__ READ objectName)
    Q_PROPERTY(QString __APP_PATH__ READ appPath)
    Q_PROPERTY(QString __DATA_PATH__ READ dataPath)
public:
    PyObj(PythonInterface* oif, QObject* parent = nullptr);

    QString objectName() const;
    QString group() const;
    QString appPath() const;
    QString dataPath() const;

    //! \name 線程鎖
    //! @{
    Q_INVOKABLE void mutexLock();    // test ✔
    Q_INVOKABLE void mutexUnlock();  // test ✔
    //! @}

    //! \name 私有數據
    //! @{
    Q_INVOKABLE void setPrivateData(
        const NS_BGMRPCObjectInterface::PyCaller* caller, const QString& key,
        const QVariant& value);  // test ✔
    Q_INVOKABLE QVariant
    privateData(const NS_BGMRPCObjectInterface::PyCaller* caller,
                const QString& key) const;  // test ✔
    //! @}

    //! \name 調用BGMRPC內部其他對象
    //! @{
    Q_INVOKABLE QVariant call(const NS_BGMRPCObjectInterface::PyCaller* caller,
                              const QString& object, const QString& method,
                              const QVariantList& args);
    /*Q_INVOKABLE void
      callLocalMethodNonblock(const NS_BGMRPCObjectInterface::PyCaller*
      caller, const QString& object, const QString& method, const
      QVariantList& args);*/
    Q_INVOKABLE QVariant call(const QString& object, const QString& method,
                              const QVariantList& args);
    Q_INVOKABLE void callNonblock(const QString& object, const QString& method,
                                  const QVariantList& args);
    //! @}

    //! \name 關聯客戶端
    //! @{
    /*!
     * \brief addRelClient
     * \param caller
     * \return
     */
    Q_INVOKABLE bool addRelClient(
        const NS_BGMRPCObjectInterface::PyCaller* caller);  // test ✔
    /*!
     * \brief removeRelClient
     * \param caller
     * \return
     */
    Q_INVOKABLE bool removeRelClient(
        const NS_BGMRPCObjectInterface::PyCaller* caller);  // test ✔
    /*!
     * \overload
     */
    Q_INVOKABLE bool removeRelClient(qint64 callerID);
    /*!
     * \brief relClients
     * \return
     */
    Q_INVOKABLE QVariantList relClients() const;  // test ✔
    /*!
     * \brief relClient
     * \param callerID
     * \return
     */
    Q_INVOKABLE NS_BGMRPCObjectInterface::PyCaller* relClient(
        qint64 callerID) const;  // test ✔
    /*!
     * \brief containsRelClient
     * \param caller
     * \return
     */
    Q_INVOKABLE bool containsRelClient(
        const NS_BGMRPCObjectInterface::PyCaller* caller) const;  // test ✔
    /*!
     * \overload
     */
    Q_INVOKABLE bool containsRelClient(qint64 callerID) const;
    /*!
     * \brief findRelClient
     * \param callback
     * \return
     */
    Q_INVOKABLE NS_BGMRPCObjectInterface::PyCaller* findRelClient(
        PythonQtObjectPtr callback);  // test ✔
    //! @}

    /*!
     * \brief 關聯客戶端退出事件
     * \param 回調函數
     */
    Q_INVOKABLE void onRelClientRemoved(PythonQtObjectPtr callback);  // test ✔

    /*!
     * \brief 向所有關聯客戶端廣播信號
     * \param 信號
     * \param 信號參數
     */
    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QVariantList& args);  // test ✔

private:
    PythonInterface* m_oif;
    PythonQtObjectPtr m_relClientRemovedHandle;
};

}  // namespace NS_BGMRPCObjectInterface

#endif  // PYOBJ_H
