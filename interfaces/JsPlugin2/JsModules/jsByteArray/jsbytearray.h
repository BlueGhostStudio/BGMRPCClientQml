#ifndef JSBYTEARRAY_H
#define JSBYTEARRAY_H

#include <QtCore>

#include "jsByteArray_global.h"
#include "jsobjfactory.h"

class JsByteArrayFactory;

class JSBYTEARRAY_EXPORT JsByteArray : public QObject {
    Q_OBJECT
public:
    JsByteArray(bool hold = false, QObject* parent = nullptr);
    JsByteArray(const QByteArray& data, bool hold = false,
                QObject* parent = nullptr);
    ~JsByteArray() {}

    Q_INVOKABLE int size() const;
    Q_INVOKABLE char at(int i) const;
    Q_INVOKABLE QJSValue mid(int pos, int len, bool hold = false) const;
    Q_INVOKABLE QJSValue toBase64(bool hold = false) const;
    Q_INVOKABLE void fromBase64(const QString& base64);

    Q_INVOKABLE bool append(const QJSValue& ba);
    Q_INVOKABLE void assign(const QByteArray& data);
    Q_INVOKABLE QString toStr() const;
    Q_INVOKABLE QByteArray data() const;

private:  // private methods
    JsByteArrayFactory* factory() const;

private:
    QByteArray m_data;
};

class JsByteArrayFactory : public JsObjFactory {
    Q_OBJECT

public:
    JsByteArrayFactory(QObject* parent = nullptr);

protected:
    QObject* constructor(const QVariant& args) const override;
};

extern "C" {
void initialModule(QJSEngine* engine);
}
#endif  // JSBYTEARRAY_H
