#ifndef PYTHONINTERFACE_H
#define PYTHONINTERFACE_H

#include <PythonQt.h>
#include <objectinterface.h>

#include <QMutex>
#include <QObject>
#include <QWaitCondition>

#include "PythonInterface_global.h"

namespace NS_BGMRPCObjectInterface {
class PYTHONINTERFACE_EXPORT PythonInterface : public ObjectInterface {
public:
    PythonInterface(QObject* parent = nullptr);

    /*QVariant callPy(const QString& name, QPointer<Caller> cli,
                    const QVariantList& args);*/
    bool loadPyFile(const QString& pyFileName);

    void mutexLock();
    void mutexUlock();

protected:
    void initial(int argc, char** argv) override;
    void registerMethods() override;
    void registerMethod(const QString& methodName);
    bool verification(QPointer<Caller> caller, const QString& method,
                      const QVariantList& args) override;
    /*void callMethod(const QString& mID, QPointer<Caller> caller,
                    const QString& methodName,
                    const QVariantList& args) override;*/

private:
    PythonQtObjectPtr m_pyMainContent;
    QString m_PWD;
    bool m_hasPyRuning;
    QWaitCondition m_waitForRun;

    QMutex m_mutex;
};

}  // namespace NS_BGMRPCObjectInterface

extern "C" {
NS_BGMRPCObjectInterface::ObjectInterface* create(/*int, char***/);
}
#endif  // PYTHONINTERFACE_H
