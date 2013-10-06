#include "datatype.h"

namespace BGMircroRPC {

returnedValue_t::returnedValue_t(const QJsonObject& v)
    : Value (v)
{
}

returnedValue_t::returnedValue_t(const QJsonArray& v)
{
    setReturnedValues (v);
}

returnedValue_t::returnedValue_t(const QString& obj,
                                 const QString& signal,
                                 const QJsonArray& args)
{
    setSignal (obj, signal, args);
}

bool returnedValue_t::isReturned() const
{
    return Value ["type"].toString () == "return";
}


bool
returnedValue_t::isSignal() const
{
    return Value ["type"].toString () == "signal";
}

void returnedValue_t::setReturnedValues(const QJsonArray& v)
{
    Value ["type"] = QString ("return");
    Value ["values"] = v;
}

void returnedValue_t::setSignal(const QString& obj,
                                const QString& signal,
                                const QJsonArray& args)
{
    Value ["type"] = QString ("signal");
    Value ["object"] = obj;
    Value ["signal"] = signal;
    Value ["args"] = args;
}

returnedValue_t::converter returnedValue_t::returnedValue () const
{
    if (isReturned ())
        return converter (Value ["values"].toArray () [0]);
    else
        return converter ();
}

returnedValue_t::converter returnedValue_t::operator [](int i) const
{
    if (isReturned ())
        return converter (Value ["values"].toArray () [i]);
    else if (isSignal ())
        return converter (Value ["args"].toArray () [i]);
    else
        return converter ();
}

int returnedValue_t::count() const
{
    if (isReturned ())
        return Value ["values"].toArray ().count ();
    else
        return Value ["args"].toArray ().count ();
}

QString returnedValue_t::signal() const
{
    QString mName;
    if (isSignal ())
        mName = Value ["signal"].toString ();

    return mName;
}

QString returnedValue_t::object() const
{
    return Value ["object"].toString ();
}

QJsonArray returnedValue_t::args() const
{
    return Value ["args"].toArray ();
}

qulonglong returnedValue_t::pID() const
{
    return (qulonglong)Value ["pID"].toDouble ();
}

args_t::args_t() : QJsonArray ()
{
}

args_t::args_t(const QJsonArray& other)
    : QJsonArray (other)
{
}

args_t& args_t::operator << (const QJsonValue& v)
{
    append (v);

    return *this;
}

}
