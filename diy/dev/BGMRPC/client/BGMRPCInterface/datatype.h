#ifndef DATATYPE_H
#define DATATYPE_H

#include <QtCore/qglobal.h>
#include <QtCore>

namespace BGMircroRPC {

class returnedValue_t
{
public:
    returnedValue_t () {}
    returnedValue_t (const QJsonObject& v);
    returnedValue_t (const QJsonArray& v);
    returnedValue_t (const QString& obj, const QString& signal,
                     const QJsonArray& args);

    bool isReturned () const;
    bool isSignal () const;

    void setReturnedValues (const QJsonArray& v);
    void setSignal (const QString& obj, const QString& signal,
                    const QJsonArray& args);

    class converter
    {
    public:
        converter () {}
        converter (const QJsonValue& v) : Value (v) {}

        operator double () const { return Value.toDouble (); }
        operator bool () const { return Value.toBool (); }
        operator QString () const { return Value.toString (); }
        operator QJsonArray () const { return Value.toArray (); }
        operator QJsonObject () const { return Value.toObject (); }
        operator QJsonValue () const { return Value; }

    private:
        QJsonValue Value;
    };

    converter returnedValue () const;
    operator double () const { return returnedValue (); }
    operator bool () const { return returnedValue (); }
    operator QString () const { return returnedValue (); }
    operator QJsonArray () const { return returnedValue (); }
    operator QJsonObject () const { return returnedValue (); }

    converter operator [] (int i) const;
    int count () const;

    QString signal () const;
    QString object () const;
    QJsonArray args () const;
    qulonglong pID () const;

private:
    QJsonObject Value;
};

class args_t : public QJsonArray
{
public:
    args_t ();
    args_t (const QJsonArray& other);

    args_t& operator << (const QJsonValue& v);
};

}

#endif // BGMRPCCOMMON_H
