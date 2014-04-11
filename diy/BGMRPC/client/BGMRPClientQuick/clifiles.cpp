#include "clifiles.h"
#include "clifile.h"

cliFiles::cliFiles(QJSEngine* e, QObject *parent) :
    QObject(parent), Engine (e)
{
}

QJSValue cliFiles::file()
{
    return Engine->newQObject (new cliFile);
}

QJSValue cliFiles::fileInfo(const QString& fileName) const
{
    return fileInfo (cliFile::fileInfo (fileName));
}

QJSValue cliFiles::fileInfo (const QFileInfo& info) const
{
    QJSValue theFileInfo_js = Engine->newObject ();
    theFileInfo_js.setProperty ("name", info.fileName ());
    theFileInfo_js.setProperty ("path", info.absolutePath ());
    theFileInfo_js.setProperty ("filePath", info.absoluteFilePath ());
    theFileInfo_js.setProperty ("size", (double)info.size ());
    theFileInfo_js.setProperty ("isExist", info.exists ());
    if (info.isDir())
        theFileInfo_js.setProperty ("type", "dir");
    else
        theFileInfo_js.setProperty ("type", "file");

    return theFileInfo_js;
}

bool cliFiles::cd(const QString& path)
{
    if (path.isEmpty ()) {
        Dir = QDir::home ();
        return true;
    } else
        return Dir.cd(path);
}

QString cliFiles::currentPath() const
{
    return Dir.absolutePath ();
}

QJSValue cliFiles::dir()
{
    QJSValue ls = Engine->newArray ();
    int i = 0;
    foreach (QFileInfo fileEntry,
             Dir.entryInfoList (QDir::Dirs
                                | QDir::Files
                                | QDir::NoDotAndDotDot, QDir::DirsFirst)) {
        ls.setProperty (i, fileInfo (fileEntry));
        i++;
    }

    return ls;
}

