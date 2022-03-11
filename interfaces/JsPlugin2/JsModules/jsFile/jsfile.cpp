#include "jsfile.h"

#include <QDebug>
#include <QDir>
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

bool
JsFile::copy(const QString& fileName, const QString& newFileName) {
    return QFile::copy(fileName, newFileName);
}

bool
JsFile::remove(const QString& fileName) {
    return QFile::remove(fileName);
}

bool
JsFile::rename(const QString& fileName, const QString& newFileName) {
    return QFile::rename(fileName, newFileName);
}

bool
JsFile::mkpath(const QString& path) {
    return QDir().mkpath(path);
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
