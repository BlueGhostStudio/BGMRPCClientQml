#include "math.h"
#include <bgmrpccommon.h>
#include <caller.h>

using namespace NS_BGMRPCObjectInterface;

Math::Math(QObject* parent) : ObjectInterface(parent)
{
    registerMethods();
    QObject::connect(this, &Math::callerExisted, [](QPointer<Caller> caller) {
        qDebug() << "caller exited" << caller->ID();
    });
    QObject::connect(this, &Math::relatedCallerExited,
                     [=](QPointer<Caller> caller) {
                         qDebug() << "关联调用者退出" << caller->ID()
                                  << privateData(caller, "name");
                     });
}

QVariant Math::plus(QPointer<Caller> /*cli*/, const QVariantList& args)
{
    //    QVariantMap t = {{"t", 123}, {"b", 234}};
    //    cli->emitSignalReady("test_signal", t);
    return args[0].toInt() + args[1].toInt();
}

QVariant Math::join(QPointer<Caller> cli, const QVariantList& args)
{
    addRelatedCaller(cli);

    setPrivateData(cli, "name", args[0].toString());

    return true;
}

QVariant Math::foreachRelatedCaller(QPointer<Caller>, const QVariantList&)
{
    findRelatedCaller([=](QPointer<Caller> cli) -> bool {
        qDebug() << privateData(cli, "name");
        /*if (cli->ID() == 4) {
            qDebug() << "found";
            return true;
        } else
            qDebug() << cli->ID();*/
        return false;
    });

    return true;
}

QVariant Math::broadcastSignal(QPointer<Caller>, const QVariantList&)
{
    emitSignal("testBroadcastSignal", QVariantList({"test1", "test2"}));

    return true;
}

QVariant Math::testThread(QPointer<Caller>, const QVariantList& args)
{
    QEventLoop loop;
    qDebug() << "111111" << args[0].toString();
    QTimer::singleShot(500, [&]() {
        qDebug() << "222222" << args[0].toString();
        loop.quit();
    });

    loop.exec();

    return args[0].toString();
}

bool Math::verification(QPointer<Caller> /*caller*/, const QString& /*method*/,
                        const QVariantList& /*args*/)
{
    /*if (method == "join")
        return true;
    else if (privateData(caller, "name").toString() == "usr1")
        return true;
    else
        return false;*/
    return true;
}

void Math::registerMethods()
{
    m_methods["plus"] = REG_METHOD(Math, plus);
    m_methods["join"] = REG_METHOD(Math, join);
    m_methods["foreachRelatedCaller"] = REG_METHOD(Math, foreachRelatedCaller);
    m_methods["broadcastSignal"] = REG_METHOD(Math, broadcastSignal);
    m_methods["testThread"] = REG_METHOD(Math, testThread);
}

ObjectInterface* create(int, char**)
{
    return new Math;
}
