#include "chatplugin.h"
#include <bgmrprocedure.h>
#include <QJsonArray>

using namespace BGMircroRPCServer;

QString chatObj::objectType() const
{
    return objType ();
}

QJsonArray chatObj::say(BGMRProcedure* p, const QJsonArray& args)
{
    QJsonArray ret;
    if (hasJoin (p)) {
        QString nickname = p->privateData (this, "nickname").toString ();

        QJsonArray message (args);
        message.prepend (nickname);
        RelProc.emitSignal (this, "said", message);

        ret.append (true);
    } else
        ret.append (false);

    return ret;
}

QJsonArray chatObj::join(BGMRProcedure* p, const QJsonArray& args)
{
    QJsonArray ret;
    if (!hasJoin (p)) {
        ret.append (true);
        ret.append (join (p, args [0].toString ()));
    } else
        ret.append (false);

    return ret;
}

QJsonArray chatObj::changeNickname(BGMRProcedure* p, const QJsonArray& args)
{
    QJsonArray ret;
    QString nickname = args [0].toString ();

    if (hasJoin (p)) {
        QString oldNickname = p->privateData (this, "nickname").toString ();
        ret.append (true);
        if (!nickname.isEmpty ()) {
            QJsonArray sigArgs;
            sigArgs.append (oldNickname);
            sigArgs.append (nickname);
            ret.append (nickname);
            p->privateData (this, "nickname") = nickname;
            RelProc.emitSignal (this, "nicknameChanged", sigArgs);
        } else
            ret.append (oldNickname);
    } else
        ret.append (false);

    return ret;
}

QString chatObj::join(BGMRProcedure* p, const QString& nick)
{
    QString nickname;

    if (RelProc.addProc (p))  {
        if (!nick.isEmpty ())
            nickname = nick;
        else
            nickname = QString ("noname%1").arg (p->pID ());

        p->privateData (this, "nickname") = nickname;

        QJsonArray sigArgs;
        sigArgs.append (nickname);
        RelProc.emitSignal (this, "joined", sigArgs);
    }

    return nickname;
}

bool chatObj::hasJoin(BGMRProcedure* p)
{
    return RelProc.procs ().contains (p->pID ());
}

// ==============

void chatAdaptor::registerMethods ()
{
    Methods ["say"] = &chatObj::say;
    Methods ["join"] = &chatObj::join;
    Methods ["changeNickname"] = &chatObj::changeNickname;
}


// ==============
BGMRObjectInterface* objCreator ()
{
    return new chatObj;
}

QString objType ()
{
    return "chat";
}
