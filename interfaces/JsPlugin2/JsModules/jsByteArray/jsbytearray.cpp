#include "jsbytearray.h"

JsByteArray::JsByteArray(bool hold, QObject* parent) : QObject(parent) {
    if (!hold) deleteLater();
}

JsByteArray::JsByteArray(const QByteArray& data, bool hold, QObject* parent)
    : QObject(parent), m_data(data) {
    if (!hold) deleteLater();
}

int
JsByteArray::size() const {
    return m_data.length();
}

char
JsByteArray::at(int i) const {
    return m_data[i];
}

QJSValue
JsByteArray::mid(int pos, int len, bool hold) const {
    return factory()->newInstance(QVariantList({ m_data.mid(pos, len), hold }));
}

QJSValue
JsByteArray::toBase64(bool hold) const {
    return factory()->newInstance(QVariantList({ m_data.toBase64(), hold }));
}

void
JsByteArray::fromBase64(const QString& base64) {
    m_data = QByteArray::fromBase64(base64.toLatin1());
}

bool
JsByteArray::append(const QJSValue& ba) {
    QObject* qObj = ba.toQObject();
    if (qObj) {
        JsByteArray* jsbaObj = qobject_cast<JsByteArray*>(qObj);
        if (jsbaObj) {
            m_data.append(jsbaObj->m_data);
            return true;
        } else
            return false;
    } else {
        QVariant variant = ba.toVariant();
        if (variant.canConvert<QByteArray>()) {
            m_data.append(variant.toByteArray());
            return true;
        } else
            return false;
    }
}

void
JsByteArray::assign(const QByteArray& data) {
    m_data = data;
}

QString
JsByteArray::toStr() const {
    return QString::fromUtf8(m_data);
}

QByteArray
JsByteArray::data() const {
    return m_data;
}

JsByteArrayFactory*
JsByteArray::factory() const {
    return qobject_cast<JsByteArrayFactory*>(qjsEngine(this)
                                                 ->globalObject()
                                                 .property("JsByteArrayFactory")
                                                 .toQObject());
}

JsByteArrayFactory::JsByteArrayFactory(QObject* parent)
    : JsObjFactory(parent, false) {}

QObject*
JsByteArrayFactory::constructor(const QVariant& args) const {
    if (args.canConvert<QVariantList>()) {
        qDebug() << "list";
        QVariantList list = args.toList();
        return new JsByteArray(list[0].toByteArray(), list[1].toBool());
    } else if (args.typeId() == QMetaType::Bool) {
        qDebug() << "bool";
        return new JsByteArray(args.toBool());
    } else if (args.canConvert<QByteArray>()) {
        qDebug() << "bytearray";
        return new JsByteArray(args.toByteArray());
    } else {
        qDebug() << "undefined";
        return new JsByteArray();
    }
    /*if (args.type() == QVariant::Bool) {
        qDebug() << "bool";
        return new JsByteArray(args.toBool());
    } else if (args.type() == QVariant::ByteArray) {
        qDebug() << "bytearray";
        return new JsByteArray(args.toByteArray());
    } else if (args.type() == QVariant::List) {
        qDebug() << "list";
        QVariantList list = args.toList();
        return new JsByteArray(list[0].toByteArray(), list[1].toBool());
    } else {
        qDebug() << "undefined";
        return new JsByteArray();
    }*/
    // return new JsByteArray(args.toByteArray());
}

void
initialModule(QJSEngine* engine) {
    registerJsType<JsByteArrayFactory>("JsByteArray", *engine);
}
