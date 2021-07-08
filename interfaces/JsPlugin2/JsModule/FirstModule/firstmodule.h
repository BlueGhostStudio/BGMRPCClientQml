#ifndef FIRSTMODULE_H
#define FIRSTMODULE_H

#include <QObject>
#include <QJSEngine>
#include "jsobjfactory.h"
#include "firstmodule_global.h"

class FirstModule : public QObject
{
    Q_OBJECT

public:
    FirstModule(QObject* parent = nullptr);

    Q_INVOKABLE void testFirstModule () const;
};

class FirstModuleFactory : public JsObjFactory {
    Q_OBJECT
public:
    FirstModuleFactory (QObject* parent = nullptr);

protected:
    QObject* constructor () const;
};

extern "C" {
void initialModule (QJSEngine* engine);
}

#endif // FIRSTMODULE_H
