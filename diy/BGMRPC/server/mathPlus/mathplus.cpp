#include "mathplus.h"

double math::plus(double a, double b)
{
    return a + b;
}

double math::minus(double a, double b)
{
    return a - b;
}



QString mathObj::objectType() const
{
    return objType ();
}

QJsonArray mathObj::plus(BGMRProcedure*,
                        const QJsonArray& args)
{
    qDebug () << "in plus";
    qDebug () << args[0].toInt ();
    QJsonArray result;
    result.append (Math.plus (args [0].toDouble (), args [1].toDouble ()));

    return result;
}

QJsonArray mathObj::minus(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray result;
    result.append (Math.minus (args [0].toDouble (), args [1].toDouble ()));

    return result;
}


void mathAdaptor::registerMethods()
{
    Methods ["plus"] = &mathObj::plus;
    Methods ["minus"] = &mathObj::minus;
}


BGMRObjectInterface* objCreator()
{
    return new mathObj;
}


QString objType()
{
    return QString ("mathDemo");
}

bool initial(BGMRObjectStorage* storage, BGMRPC*) // NOTE 测试
{
    storage->installObject ("math", new mathObj);

    return true;
}
