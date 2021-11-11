#ifndef JSTIMER_H
#define JSTIMER_H

#include <jsobjfactory.h>

#include <QJSEngine>
#include <QObject>
#include <QtCore>

#include "jstimer_global.h"

class JsTimer : public QObject {
    Q_OBJECT

public:
    JsTimer(QObject* parent = nullptr);

    Q_INVOKABLE void setTimeOut(int mesc, const QJSValue& fun);
    Q_INVOKABLE void setInterval(int mesc, const QJSValue& fun);

private:
    QTimer Timer;
    QJSValue Callback;
};

class JsTimerFactory : public JsObjFactory {
    Q_OBJECT

public:
    JsTimerFactory(QObject* parent = nullptr);

protected:
    QObject* constructor(const QVariant&) const override;
};

extern "C" {
void initialModule(QJSEngine* engine);
}

#endif  // JSTIMER_H
