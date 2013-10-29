#include "chatplugin.h"
#include <bgmrprocedure.h>
#include <QJsonArray>

using namespace BGMircroRPCServer;

chatObj::chatObj(QObject* parent)
    : QObject (parent)
{
//    connect (&RelProc, SIGNAL(removedProc(BGMRProcedure*)),
//             SLOT(leaved(BGMRProcedure*)));
}

QString chatObj::objectType() const
{
    return objType ();
}

QJsonArray chatObj::say(BGMRProcedure* p, const QJsonArray& args)
{
    QJsonArray ret;
    if (hasJoined (p)) {
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
    if (!hasJoined (p)) {
        ret.append (true);
        ret.append (join (p, args [0].toString ()));
    } else
        ret.append (false);

    return ret;
}

QJsonArray chatObj::hasJoined(BGMRProcedure* p, const QJsonArray&)
{
    QJsonArray ret;
    ret.append (hasJoined (p));
    return ret;
}

QJsonArray chatObj::changeNickname(BGMRProcedure* p, const QJsonArray& args)
{
    QJsonArray ret;
    QString nickname = args [0].toString ();

    if (hasJoined (p)) {
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

QJsonArray chatObj::whoList(BGMRProcedure* p, const QJsonArray&)
{
    QJsonArray ret;
    if (hasJoined (p)) {
        ret.append (true);
        QJsonArray list;
        QMap < qulonglong, BGMRProcedure* > procs = RelProc.procs ();
        QMap < qulonglong, BGMRProcedure* >::const_iterator it;
        for (it = procs.constBegin (); it != procs.constEnd (); ++it)
            list.append (it.value ()->privateData (this, "nickname"));

        if (!list.isEmpty ())
            ret.append (list);
    } else
        ret.append (false);
    return ret;
}

QJsonArray chatObj::leave(BGMRProcedure* p, const QJsonArray&)
{
    QJsonArray ret;
    if (hasJoined (p)) {
        ret.append (RelProc.removeProc (p->pID ()));
    } else
        ret.append (false);

    return ret;
}

void chatObj::leaved(BGMRProcedure* proc)
{
    if (proc) {
        QJsonArray sigArgs;
        sigArgs.append (proc->privateData (this, "nickname"));
        RelProc.emitSignal (this, "leaved", sigArgs);
    }
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
        qDebug () << "addProc";
    }

    return nickname;
}

bool chatObj::hasJoined(BGMRProcedure* p)
{
    return RelProc.procs ().contains (p->pID ());
}

// ==============

void chatAdaptor::registerMethods ()
{
    Methods ["say"] = &chatObj::say;
    Methods ["join"] = &chatObj::join;
    Methods ["hasJoined"] = &chatObj::hasJoined;
    Methods ["leave"] = &chatObj::leave;
    Methods ["changeNickname"] = &chatObj::changeNickname;
    Methods ["whoList"] = &chatObj::whoList;
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
