#ifndef JSFILEPROTO_H
#define JSFILEPROTO_H

#include <QObject>
#include <QFile>
#include <bgmrpc.h>
#include "jsobjectclass.h"

using namespace BGMircroRPCServer;

typedef QFile* filePtr;

class jsFileProto;

template <>
class protoTypeInfo < jsFileProto >
{
public:
    typedef filePtr dataType;
    static QString className () { return "JSFile"; }
    static bool isNull (dataType d) { return !d; }
    static filePtr nullData () { return NULL; }
    static dataType newObject () {
        QFile* file = new QFile (0);
        file->deleteLater ();
        return file;
    }
};

class jsFileProto:public QObject, public QScriptable
{
    Q_OBJECT

public:
    jsFileProto (QObject* parent = 0);
    ~jsFileProto ();

    Q_INVOKABLE bool open (const QString& fileName, int mode);
    Q_INVOKABLE void close ();
    Q_INVOKABLE QByteArray readAll ();

private:
    filePtr thisFileObj () const;
    QString dataRootDir () const;
};

typedef jsObjectClass < jsFileProto > jsFileClass;
Q_DECLARE_METATYPE (const QFile*)
Q_DECLARE_METATYPE (QFile*)
Q_DECLARE_METATYPE (jsFileClass*)

#endif // JSFILEPROTO_H
