#ifndef PYOBJ_H
#define PYOBJ_H

#include "pythoninterface.h"
#include <QObject>
#include <objectinterface.h>

namespace NS_BGMRPCObjectInterface
{
class PyCaller : public QObject
{
    Q_OBJECT

    Q_PROPERTY(qint64 ID READ ID)
public:
    PyCaller(QPointer<Caller> caller, QObject* parent = nullptr);
    ~PyCaller();

    qint64 ID() const;
    Q_INVOKABLE bool online() const; // test ✔
    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QVariant& args) const; // test ✔

    Q_INVOKABLE NS_BGMRPCObjectInterface::PyCaller* clone() const; // test ✔
    Q_INVOKABLE void destory();                                    // test ✔

    QPointer<Caller> caller() const;

private:
    QPointer<Caller> m_caller;
};

class PyObj : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString __NAME__ READ objectName)
public:
    PyObj(PythonInterface* oif, QObject* parent = nullptr);

    QString objectName() const;

    Q_INVOKABLE void mutexLock();   // test ✔
    Q_INVOKABLE void mutexUnlock(); // test ✔

    Q_INVOKABLE void
    setPrivateData(const NS_BGMRPCObjectInterface::PyCaller* caller,
                   const QString& key, const QVariant& value); // test ✔
    Q_INVOKABLE QVariant
    privateData(const NS_BGMRPCObjectInterface::PyCaller* caller,
                const QString& key) const; // test ✔

    /*!
     * \brief addRelClient
     * \param caller
     * \return
     */
    Q_INVOKABLE bool
    addRelClient(const NS_BGMRPCObjectInterface::PyCaller* caller); // test ✔
    /*!
     * \brief removeRelClient
     * \param caller
     * \return
     */
    Q_INVOKABLE bool
    removeRelClient(const NS_BGMRPCObjectInterface::PyCaller* caller); // test ✔
    /*!
     * \brief relClients
     * \return
     */
    Q_INVOKABLE QVariantList relClients() const; // test ✔
    /*!
     * \brief relClient
     * \param callerID
     * \return
     */
    Q_INVOKABLE NS_BGMRPCObjectInterface::PyCaller*
    relClient(qint64 callerID) const; // test ✔
    /*!
     * \brief containsRelClient
     * \param caller
     * \return
     */
    Q_INVOKABLE bool containsRelClient(
        const NS_BGMRPCObjectInterface::PyCaller* caller) const; // test ✔
    /*!
     * \brief findRelClient
     * \param callback
     * \return
     */
    Q_INVOKABLE NS_BGMRPCObjectInterface::PyCaller*
    findRelClient(PythonQtObjectPtr callback); // test ✔

    Q_INVOKABLE void onRelClientRemoved(PythonQtObjectPtr callback); // test ✔

    Q_INVOKABLE void emitSignal(const QString& signal,
                                const QVariantList& args); // test ✔

private:
    PythonInterface* m_oif;
    PythonQtObjectPtr m_relClientRemovedHandle;
};

} // namespace NS_BGMRPCObjectInterface

#endif // PYOBJ_H
