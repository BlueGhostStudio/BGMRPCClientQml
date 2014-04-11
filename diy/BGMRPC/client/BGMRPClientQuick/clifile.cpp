#include <QUrl>
#include <QJSValue>
#include "clifile.h"

cliFile::cliFile(QObject *parent) :
    QObject(parent)
{
}

void cliFile::__destroy()
{
    deleteLater ();
}

bool cliFile::open(const QString& file)
{
    QUrl theFileUrl (file);
    QString filePath;
    if (theFileUrl.isRelative ())
        filePath = file;
    else
        filePath = theFileUrl.toLocalFile ();
    QFileInfo fileInfo (filePath);
    FileName = fileInfo.fileName ();
    FilePath = fileInfo.path ();

    File.setFileName (filePath);
    return File.open (QIODevice::ReadWrite);
}

void cliFile::close()
{
    File.close ();
}

QByteArray cliFile::readAll()
{
    return File.readAll ();
}

QByteArray cliFile::read(qint64 maxLen)
{
    return File.read (maxLen);
}

qint64 cliFile::write(const QByteArray& data)
{
    return File.write (data);
}

qint64 cliFile::fileSize()
{
    return File.size ();
}

QString cliFile::fileName() const
{
    return FileName;
}

QString cliFile::filePath () const
{
    return FilePath;
}

bool cliFile::atEnd()
{
    return File.atEnd ();
}

QFileInfo cliFile::fileInfo(const QString& file)
{
    QUrl theFileUrl (file);
    QString filePath = theFileUrl.toLocalFile ();

    return QFileInfo (filePath);
}
