#include <QJSValueIterator>
#include "step.h"
#include "bgmrpc.h"

step::step(QQuickItem* parent)
    : QQuickItem (parent)
{
}

void step::doneCallBack(const QJSValue& rets)
{
    if (JSOnDone.isCallable ())
        JSOnDone.call (QJSValueList () << rets);
}

step* step::done(const QJSValue& d)
{
    JSOnDone = d;

    return this;
}

//==========

_RStep::_RStep(QQuickItem* parent)
    : step (parent)
{
}

_RStep::_RStep(const QString& o, const QString& m, QQuickItem* parent)
    : step (parent), Object (o), Method (m)
{
}

QString _RStep::object() const
{
    return Object;
}

void _RStep::setObject(const QString& o)
{
    Object = o;
}

QString _RStep::method() const
{
    return Method;
}

void _RStep::setMethod(const QString& m)
{
    Method = m;
}

void _RStep::call(const QJSValue& args)
{
    BGMRPC* RPC = ownRPC ();
    if (RPC)
        RPC->call (this, Object, Method, args);
}

BGMRPC*_RStep::ownRPC()
{
    return qobject_cast < BGMRPC* > (parentItem ()->parentItem ());
}


_JSStep::_JSStep(QQuickItem* parent)
    : _RStep (parent)
{
}

_JSStep::_JSStep(const QString& o, const QString& m, QQuickItem* parent)
    : _RStep (o, m, parent)
{
}

void _JSStep::call(const QJSValue& args)
{
    BGMRPC* RPC = ownRPC ();
    if (RPC) {
        QJSValue jsArgs = engine->newArray ();
        jsArgs.setProperty (0, Method);

        int len = args.property ("length").toInt ();
        for (int i = 0; i < len; i++)
            jsArgs.setProperty (i + 1, args.property (i));

        RPC->call (this, Object, "js", jsArgs);
    }
}


_LStep::_LStep(QQuickItem* parent)
    : step (parent)
{
}

QJSValue _LStep::flow() const
{
    return Fun;
}

void _LStep::setFlow(const QJSValue& f)
{
    Fun = f;
}

void _LStep::call(const QJSValue& args)
{
    if (Fun.isCallable ()) {
        QJSValueList jsArgs;
        int len = args.property ("length").toInt ();

        for (int i = 0; i < len; i++)
            jsArgs << args.property (i);

        doneCallBack (Fun.call (jsArgs));
    }
}
