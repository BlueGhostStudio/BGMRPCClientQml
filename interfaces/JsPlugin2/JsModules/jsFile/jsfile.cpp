#include "jsfile.h"

#include <QDebug>
#include <QFile>

JsFile::JsFile(QObject* parent) : QObject(parent) {}

bool
JsFile::exists(const QString& fileName) {
    return QFile::exists(fileName);
}

bool
JsFile::removeFile(const QString& fileName) {
    return QFile::remove(fileName);
}

bool
JsFile::writeFile(const QString& fileName, const QByteArray& data) {
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(data);
        file.close();
        return true;
    } else
        return false;
}

QByteArray
JsFile::readFile(const QString& fileName) {
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        return data;
    } else
        return QByteArray();
}

JsFileFactory::JsFileFactory(QObject* parent) : JsObjFactory(parent, false) {}

QObject*
JsFileFactory::constructor(const QVariant&) const {
    qDebug() << "new file obj";
    return new JsFile();
}

void
initialModule(QJSEngine* engine) {
    qDebug() << "Load jsFile Module";
    registerJsType<JsFileFactory>("JsFile", *engine);
}
