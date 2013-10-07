#include "rpcfileplugin.h"
#include <bgmrpc.h>

using namespace BGMircroRPCServer;


QString RPCFileObj::objectType() const
{
    return objType ();
}

QJsonArray RPCFileObj::connect(BGMRProcedure* p, const QJsonArray&)
{
    RelProcs.addProc (p);
    return QJsonArray ();
}

QJsonArray RPCFileObj::open(BGMRProcedure* p, const QJsonArray& args)
{
    //__socket* tcpSocket = p->detachSocket ();
    fileStream* theFileStream = new fileStream (this, p->detachSocket ());

    QString fileName = getPath (args[2].toString ()) + args [0].toString ();
    QString op = args [1].toString ();

    theFileStream->openFile (fileName, op);

    return QJsonArray ();
}

QJsonArray RPCFileObj::cp(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QString oldName = getPath (args [2].toString ()) + args [0].toString ();
    QString newName = getPath (args [2].toString ()) + args [1].toString ();
    if (QFile::exists (newName) && args [3].toBool (false))
        QFile::remove (newName);

    ret.append (QFile::copy (oldName, newName));

    return ret;
}

QJsonArray RPCFileObj::rm(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QString fileName = getPath (args [1].toString ()) + args [0].toString ();
    ret.append (QFile::remove (fileName));

    return ret;
}

QJsonArray RPCFileObj::rename(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QString oldName = getPath (args [2].toString ()) + args [0].toString ();
    QString newName = getPath (args [2].toString ()) + args [1].toString ();
    ret.append (QFile::rename (oldName, newName));

    return ret;
}

QJsonArray RPCFileObj::dir (BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;

    QDir theDir (getPath (args[1].toString ()) + args [0].toString ());
    QStringList ls;
    foreach (QFileInfo theEntry, theDir.entryInfoList ()) {
        if (theEntry.isDir ())
            ls.append ("[" + theEntry.fileName () + "]");
        else
            ls.append (theEntry.fileName () + "  " + QString::number (theEntry.size ()));
    }

    ret.append (QJsonArray::fromStringList (ls));

    return ret;
}

QJsonArray RPCFileObj::mkdir(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QDir theDir (getPath (args[1].toString ()));
    ret.append (theDir.mkpath (args [0].toString ()));

    return ret;
}

QJsonArray RPCFileObj::rmdir(BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray ret;
    QDir theDir (getPath (args[1].toString ()));
    ret.append (theDir.rmdir (args [0].toString ()));

    return ret;
}

QJsonArray RPCFileObj::setPath (BGMRProcedure*, const QJsonArray& args)
{
    QJsonArray trueRet;
    QJsonArray falseRet;
    trueRet.append (true);
    falseRet.append (false);

    QString pathID = args [0].toString ();
    if (pathID.isEmpty ())
        return falseRet;

    QString path = args [1].toString ();
    if (path.isEmpty ())
        return falseRet;

    path.replace (QRegExp ("^\\s*~"), QDir::homePath ());
    QDir theDir;
    if (path [0] == '/')
        theDir.setPath (path);
    else
        theDir.setPath (rootPath () + path);

    if (!theDir.exists ())
        return falseRet;

    path = theDir.absolutePath () + '/';
    BGMRPC::Settings->setValue (QString("path/%1").arg (pathID), path);

    return trueRet;
}

QJsonArray RPCFileObj::paths(BGMRProcedure*, const QJsonArray&)
{
    QJsonArray ret;

    BGMRPC::Settings->beginGroup ("path");
    QStringList pathList;
    foreach (QString key, BGMRPC::Settings->childKeys ()) {
        QString path = BGMRPC::Settings->value (key).toString ();
        pathList.append (QString ("%1=%2").arg (key).arg (path));
    }
    BGMRPC::Settings->endGroup ();

    ret.append (QJsonArray::fromStringList (pathList));
    return ret;
}

QJsonArray RPCFileObj::removePath(BGMRProcedure*, const QJsonArray& args)
{
    QString pathID = args [0].toString ();
    if (!pathID.isEmpty ()) {
        BGMRPC::Settings->beginGroup ("path");
        BGMRPC::Settings->remove (pathID);
        BGMRPC::Settings->endGroup ();
    }

    return QJsonArray ();
}

QJsonArray RPCFileObj::rootPath(BGMRProcedure*, const QJsonArray&)
{
    QString theRootPath = BGMRPC::Settings->value ("rootDir", "~/.BGMR").toString ();
    QJsonArray ret;
    ret.append (theRootPath);

    return ret;
}

BGMRProcedure* RPCFileObj::proc(quint16 id) const
{
    return RelProcs.procs () [id];
}

QString RPCFileObj::rootPath ()
{
    QString theRootPath = BGMRPC::Settings->value ("rootDir", "~/.BGMR").toString ();
    theRootPath.replace (QRegExp ("^\\s*~"), QDir::homePath ());

	if (!theRootPath.contains (QRegExp ("/\\s*$")))
		theRootPath += "/";

    return theRootPath;
}

QString RPCFileObj::getPath(const QString& pathID)
{
    QString path;
    if (!pathID.isEmpty ()) {
        //QSettings setting("BG", "BGMR");
        path = BGMRPC::Settings->value (QString("path/%1").arg (pathID)).toString ();
    }
    if (path.isEmpty ())
        path = rootPath ();

    return path;
}

void RPCFileAdaptor::registerMethods()
{
    Methods ["connect"] = &RPCFileObj::connect;
    Methods ["open"] = &RPCFileObj::open;
    Methods ["cp"] = &RPCFileObj::cp;
    Methods ["rm"] = &RPCFileObj::rm;
    Methods ["rename"] = &RPCFileObj::rename;
    Methods ["dir"] = &RPCFileObj::dir;
    Methods ["mkdir"] = &RPCFileObj::mkdir;
    Methods ["rmdir"] = &RPCFileObj::rmdir;
    Methods ["setPath"] = &RPCFileObj::setPath;
    Methods ["paths"] = &RPCFileObj::paths;
    Methods ["removePath"] = &RPCFileObj::removePath;
    Methods ["rootPath"] = &RPCFileObj::rootPath;
}

BGMRObjectInterface* objCreator ()
{
    return NULL;
}

QString objType ()
{
    return QString ("file");
}

bool initial (BGMRObjectStorage* storage, BGMRPC*)
{
    storage->installObject ("file", new RPCFileObj);

    return true;
}
