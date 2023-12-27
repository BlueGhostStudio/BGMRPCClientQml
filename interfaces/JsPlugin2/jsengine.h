#ifndef JSENGINE_H
#define JSENGINE_H

#include <objectinterface.h>

#include <QJSEngine>
#include <QObject>
#include <QtCore>

#include "JsPlugin2_global.h"
namespace NS_BGMRPCObjectInterface {
class JsJSObj;

class JSPLUGIN2_EXPORT JsEngine : public ObjectInterface {
    Q_OBJECT

public:
    JsEngine(QObject* parent = nullptr);

    QVariant callJs(const QString& name, QPointer<Caller> cli,
                    const QVariantList& args);
    bool loadJsFile(const QString& jsFileName);

    QString modulesPath() const;
    QString PWD() const;

protected:
    bool initial(int argc, char** argv) override;

    void registerMethods() override;
    void registerMethod(const QString& methodName);
    bool verification(QPointer<Caller> caller, const QString& method,
                      const QVariantList& args) override;

private:
    void loadModule(const QString& module);
    QJSValue variant2JsValue(const QVariant& var);

private:
    QJSEngine* m_jsEngine;
    QJSValue m_jsVerificationFun;
    QString m_PWD;
    QStringList m_loadedModules;
    QString m_jsModulesPath;

    //    QMap<QString, QJSValue> m_jsMethods;
    QMutex m_mutex;

    friend JsJSObj;
};
}  // namespace NS_BGMRPCObjectInterface

extern "C" {
NS_BGMRPCObjectInterface::ObjectInterface* create(/*int, char***/);
}
#endif  // JSENGINE_H
