#ifndef JSOBJFACTORY_H
#define JSOBJFACTORY_H

#include <QJSEngine>
#include <QObject>

class JsObjFactory : public QObject {
    Q_OBJECT
public:
    explicit JsObjFactory(QObject* parent = nullptr, bool autoDestory = true);

    Q_INVOKABLE QJSValue newInstance(const QVariant& args) const;

signals:

public slots:
protected:
    virtual QObject* constructor(const QVariant&) const = 0;
    bool AutoDestory;
};

template <typename FT>
void
registerJsType(const QString& typeName, QJSEngine& engine) {
    FT* factory = new FT;
    // QString factoryName = factory->factoryName ();
    QString factoryName = typeName + "Factory";
    engine.globalObject().setProperty(factoryName, engine.newQObject(factory));
    engine.evaluate(
        QString(R"JSS( function %1 () { )JSS"
                R"JSS(     var args = arguments.length == 0 )JSS"
                R"JSS(         ? undefined )JSS"
                R"JSS(         : (arguments.length == 1 )JSS"
                R"JSS(                ? arguments[0] )JSS"
                R"JSS(                : Array.apply(null, arguments) )JSS"
                R"JSS(           ); )JSS"
                R"JSS(     return %2.newInstance (args); )JSS"
                R"JSS( } )JSS")
            .arg(typeName)
            .arg(factoryName));
}

#endif  // JSOBJFACTORY_H
