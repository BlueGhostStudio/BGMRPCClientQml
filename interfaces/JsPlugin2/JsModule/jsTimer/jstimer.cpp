#include "jstimer.h"
#include <QDebug>

JsTimer::JsTimer(QObject* parent)
    : QObject (parent)
{
}

void JsTimer::setTimeOut(int mesc, const QJSValue& fun)
{
    qDebug () << "in setTimeOut" << mesc;
    if (!fun.isCallable ())
        return;

    Callback = fun;
    QTimer::singleShot (mesc, [&] () {
        Callback.call ();
    });
}

void JsTimer::setInterval(int mesc, const QJSValue& fun)
{
    if (Timer.isActive ())
        Timer.stop ();

    if (!fun.isCallable ())
        return;

    Callback = fun;
    Timer.setInterval (mesc);
    QObject::connect (&Timer, &QTimer::timeout,
                      [&] () {
        Callback.call ();
    });

    Timer.start ();
}

JsTimerFactory::JsTimerFactory(QObject* parent)
    : JsObjFactory (parent, false)
{

}

QObject* JsTimerFactory::constructor() const
{
    return new JsTimer;
}

void initialModule (QJSEngine* engine)
{
    qDebug () << "load jsTimer module";
    registerJsType < JsTimerFactory > ("JsTimer", *engine);
}
