#include "rpcfileplugin.h"
#include <bgmrpc.h>

using namespace BGMircroRPCServer;


QString RPCFileObj::objectType() const
{
    return objType ();
}

QJsonArray RPCFileObj::connect(BGMRClient* p, const QJsonArray&)
{
    RelProcs.addRelatedClient (p);
    return QJsonArray ();
}

QJsonArray RPCFileObj::download(BGMRClient* p, const QJsonArray& args)
{
    fileStream* newFileStream = new fileStream (p);
    newFileStream->sendFile (rootPath () + args [0].toString ());

    return QJsonArray ();
}

QJsonArray RPCFileObj::upload(BGMRClient* p, const QJsonArray& args)
{
    fileStream* newFileStream = new fileStream (p);
    newFileStream->receiveFile (rootPath () + args [0].toString ());

    return QJsonArray ();
}

QJsonArray RPCFileObj::cp(BGMRClient*, const QJsonArray& args)
{
    QJsonArray ret;
    QString oldName = getPath (args [2].toString ()) + args [0].toString ();
    QString newName = getPath (args [2].toString ()) + args [1].toString ();
    if (QFile::exists (newName) && args [3].toBool (false))
        QFile::remove (newName);

    ret.append (QFile::copy (oldName, newName));

    return ret;
}

QJsonArray RPCFileObj::rm(BGMRClient*, const QJsonArray& args)
{
    QJsonArray ret;
    QString fileName = getPath (args [1].toString ()) + args [0].toString ();
    ret.append (QFile::remove (fileName));

    return ret;
}

QJsonArray RPCFileObj::rename(BGMRClient*, const QJsonArray& args)
{
    QJsonArray ret;
    QString oldName = getPath (args [2].toString ()) + args [0].toString ();
    QString newName = getPath (args [2].toString ()) + args [1].toString ();
    ret.append (QFile::rename (oldName, newName));

    return ret;
}

QJsonArray RPCFileObj::dir (BGMRClient*, const QJsonArray& args)
{
    QJsonArray ret;

    QString path (args [0].toString());
    if (path.contains(QRegExp ("^\\s*/?\\.\\."))) {
        ret.append(false);
        return ret;
    } else
        ret.append(true);

    QString theBasePath (getPath (args[1].toString ()));
    path = theBasePath + path;

    QDir theDir (path);
    QJsonArray ls;

    foreach (QFileInfo theEntry, theDir.entryInfoList (QDir::Dirs | QDir::Files | QDir::NoDotAndDotDot, QDir::DirsFirst)) {
        QJsonObject entry_json;
        entry_json ["name"] = theEntry.fileName ();
        if (theEntry.isDir ())
            entry_json ["type"] = QString ("dir");
        else {
            entry_json ["type"] = QString ("file");
            entry_json ["size"] = QString::number (theEntry.size ());
        }
        ls.append (entry_json);
    }

    ret.append (ls);

    QDir theBaseDir (theBasePath);
    ret.append (theBaseDir.relativeFilePath (path));

    return ret;
}

QJsonArray RPCFileObj::mkdir(BGMRClient*, const QJsonArray& args)
{
    QJsonArray ret;
    QDir theDir (getPath (args[1].toString ()));
    ret.append (theDir.mkpath (args [0].toString ()));

    return ret;
}

QJsonArray RPCFileObj::rmdir(BGMRClient*, const QJsonArray& args)
{
    QJsonArray ret;
    QDir theDir (getPath (args[1].toString ()));
    ret.append (theDir.rmdir (args [0].toString ()));

    return ret;
}

QJsonArray RPCFileObj::setPath (BGMRClient*, const QJsonArray& args)
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

QJsonArray RPCFileObj::paths(BGMRClient*, const QJsonArray&)
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

QJsonArray RPCFileObj::removePath(BGMRClient*, const QJsonArray& args)
{
    QString pathID = args [0].toString ();
    if (!pathID.isEmpty ()) {
        BGMRPC::Settings->beginGroup ("path");
        BGMRPC::Settings->remove (pathID);
        BGMRPC::Settings->endGroup ();
    }

    return QJsonArray ();
}

QJsonArray RPCFileObj::rootPath(BGMRClient*, const QJsonArray&)
{
    QString theRootPath = BGMRPC::Settings->value ("rootDir", "~/.BGMR").toString ();
    QJsonArray ret;
    ret.append (theRootPath);

    return ret;
}

BGMRClient* RPCFileObj::proc(quint16 id) const
{
    return RelProcs.relatedClients () [id];
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
    Methods ["download"] = &RPCFileObj::download;
    Methods ["upload"] = &RPCFileObj::upload;
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
