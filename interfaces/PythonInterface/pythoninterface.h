#ifndef PYTHONINTERFACE_H
#define PYTHONINTERFACE_H

#include "PythonInterface_global.h"
#include <PythonQt.h>
#include <QMutex>
#include <QObject>
#include <objectinterface.h>

namespace NS_BGMRPCObjectInterface
{
class PYTHONINTERFACE_EXPORT PythonInterface : public ObjectInterface
{
public:
    PythonInterface(QObject* parent = nullptr);

    /*QVariant callPy(const QString& name, QPointer<Caller> cli,
                    const QVariantList& args);*/
    bool loadPyFile(const QString& pyFileName);

    void mutexLock();
    void mutexUlock();

protected:
    void registerMethods() override;
    void registerMethod(const QString& methodName);
    bool verification(QPointer<Caller> caller, const QString& method,
                      const QVariantList& args) override;

private:
    PythonQtObjectPtr m_pyMainContent;
    QString m_PWD;

    QMutex m_mutex;
};

} // namespace NS_BGMRPCObjectInterface

extern "C" {
NS_BGMRPCObjectInterface::ObjectInterface* create(int, char**);
}
#endif // PYTHONINTERFACE_H
