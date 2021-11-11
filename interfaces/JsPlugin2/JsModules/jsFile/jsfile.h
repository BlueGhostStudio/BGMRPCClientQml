#ifndef JSFILE_H
#define JSFILE_H

#include <QtCore>

#include "jsFile_global.h"
#include "jsobjfactory.h"

class JSFILE_EXPORT JsFile : public QObject {
    Q_OBJECT
public:
    JsFile(QObject* parent = nullptr);
    ~JsFile() {}

    Q_INVOKABLE bool exists(const QString& fileName);
    Q_INVOKABLE bool removeFile(const QString& fileName);
    Q_INVOKABLE bool writeFile(const QString& fileName, const QByteArray& data);
    Q_INVOKABLE QByteArray readFile(const QString& fileName);
};

class JsFileFactory : public JsObjFactory {
    Q_OBJECT

public:
    JsFileFactory(QObject* parent = nullptr);

protected:
    QObject* constructor(const QVariant&) const override;
};

extern "C" {
void initialModule(QJSEngine* engine);
}

#endif  // JSFILE_H
