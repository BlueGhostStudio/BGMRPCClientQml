#ifndef JSOBJFACTORY_H
#define JSOBJFACTORY_H

#include <QObject>
#include <QJSEngine>

class JsObjFactory : public QObject
{
    Q_OBJECT
public:
    explicit JsObjFactory(QObject *parent = nullptr,
                          bool autoDestory = true);

    Q_INVOKABLE QJSValue newInstance () const;

signals:

public slots:
protected:
    virtual QObject* constructor () const = 0;
    bool AutoDestory;
};

template < typename FT >
void registerJsType (const QString& typeName, QJSEngine& engine) {
    FT* factory = new FT;
    //QString factoryName = factory->factoryName ();
    QString factoryName = typeName + "Factory";
    engine.globalObject().setProperty (factoryName,
                                       engine.newQObject(factory));
    engine.evaluate(QString (R"JSS( function %1 () { )JSS"
                             R"JSS(     return %2.newInstance (); )JSS"
                             R"JSS( } )JSS").arg(typeName).arg(factoryName));
}

#endif // JSOBJFACTORY_H
