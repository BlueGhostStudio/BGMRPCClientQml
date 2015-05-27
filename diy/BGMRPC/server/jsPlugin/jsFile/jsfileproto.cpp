#include "jsfileproto.h"

jsFileProto::jsFileProto(QObject* parent)
    : QObject (parent)
{
}

jsFileProto::~jsFileProto()
{
}

bool jsFileProto::open(const QString& fileName, int mode)
{
    QFile* file = thisFileObj ();
    file->setFileName (dataRootDir () + fileName);
    return file->open ((QIODevice::OpenMode)mode);
}

void jsFileProto::close()
{
    thisFileObj ()->close ();
}

QByteArray jsFileProto::readAll()
{
    return thisFileObj ()->readAll ();
}

filePtr jsFileProto::thisFileObj() const
{
    return qvariant_cast < filePtr > (thisObject ().data ().toVariant ());
}

QString jsFileProto::dataRootDir() const
{
    return BGMRPC::Settings->value ("rootDir", "~/.BGMRPC").toString ()
            + "/data/";
}
