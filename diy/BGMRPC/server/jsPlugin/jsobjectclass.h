#ifndef JSOBJECTCLASS_H
#define JSOBJECTCLASS_H

#include <QObject>
#include <QtScript>
#include <typeinfo>

template < typename PT >
class protoTypeInfo {};

template < typename PT >
class jsObjectClass : public QScriptClass
{
public:
    typedef typename protoTypeInfo < PT >::dataType dataType;

    explicit jsObjectClass(QScriptEngine* engine);

    QScriptValue newInstance(const dataType& data);
    QString name() const;
    QScriptValue construct() const;
    QScriptValue prototype() const;

private:
    static QScriptValue construct(QScriptContext* ctx, QScriptEngine*);

    QString ClassName;
    QScriptValue Ctor;
    QScriptValue Proto;
};

template < typename PT >
jsObjectClass < PT >::jsObjectClass(QScriptEngine* engine)
    : QScriptClass (engine)
{
    Proto = engine->newQObject (new PT,
                                QScriptEngine::ScriptOwnership,
                                QScriptEngine::SkipMethodsInEnumeration
                                | QScriptEngine::ExcludeSuperClassMethods
                                | QScriptEngine::ExcludeSuperClassProperties);
    Proto.setPrototype (engine->globalObject ().property ("Object").property("prototype"));
    Ctor = engine->newFunction (construct, Proto);
    Ctor.setData (engine->toScriptValue (this));
}

template < typename PT >
QScriptValue
jsObjectClass < PT >::newInstance (const dataType& data)
{
    QScriptValue jsData = engine ()->newVariant (QVariant::fromValue (data));

    return engine ()->newObject (this, jsData);
}

template < typename PT >
QString
jsObjectClass < PT >::name () const
{
    return protoTypeInfo < PT >::className ();
}

template < typename PT >
QScriptValue
jsObjectClass < PT >::construct () const
{
    return Ctor;
}

template < typename PT >
QScriptValue
jsObjectClass < PT >::prototype () const
{
    return Proto;
}

template < typename PT >
QScriptValue
jsObjectClass < PT >::construct (QScriptContext* ctx, QScriptEngine*)
{
    jsObjectClass < PT >* cls = qscriptvalue_cast < jsObjectClass < PT >* > (ctx->callee ().data ());
    if (cls) {
        QScriptValue arg = ctx->argument (0);
        if (arg.instanceOf (ctx->callee ()))
            return cls->newInstance (qscriptvalue_cast < dataType >(arg));
        else
            return cls->newInstance (protoTypeInfo < PT >::newObject ());

    } else
        return QScriptValue ();
}

#endif // JSOBJECTCLASS_H
