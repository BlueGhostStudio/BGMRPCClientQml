#include <QDebug>
#include <jsobjfactory.h>
#include "firstmodule.h"


FirstModule::FirstModule(QObject* parent)
    : QObject (parent)
{
}

void FirstModule::testFirstModule() const
{
    qDebug () << ">>>>>>>>> first module ok <<<<<<<<<<<<";
}

FirstModuleFactory::FirstModuleFactory(QObject* parent)
    : JsObjFactory (parent)
{
    
}

QObject* FirstModuleFactory::constructor() const
{
    return new FirstModule ();
}

void initialModule(QJSEngine* engine)
{
    registerJsType < FirstModuleFactory > ("firstModule", *engine);
}
