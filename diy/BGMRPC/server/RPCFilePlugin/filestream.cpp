#include "filestream.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <rpcfileplugin.h>

using namespace BGMircroRPCServer;



/*fileStream::fileStream(RPCFileObj* fileObj, __socket* socket,
                       const QString& fileName, const QString& op,
                       qulonglong signalChannel, QObject* parent)
    : QObject (parent), FileObj (fileObj), FileSocket (socket),
      SignalChannel (signalChannel)
{
    if (!FileSocket)
        qDebug () << "is NULL";
    openFile (fileName, op);
    connect (FileSocket, SIGNAL(readyRead()),
             this, SLOT(writeFile()));
    connect (FileSocket, SIGNAL(bytesWritten(qint64)),
             this, SLOT(transFile(qint64)));
    connect (FileSocket, SIGNAL(disconnected()),
             this, SLOT(closeFile()));
}*/

fileStream::fileStream(RPCFileObj* fileObj,
                       __socket* socket,
                       QObject* parent)
    : QObject (parent), FileObj (fileObj), FileSocket(socket)
{
    connect (FileSocket, SIGNAL(disconnected()),
             this, SLOT(closeFile()));
}

void fileStream::openFile(const QString& fileName, const QString& op)
{
    File.setFileName (fileName);
    char ok = 0;
    bool isExists = File.exists ();

    if (op == "wx" && isExists) {
        emitFileSignal (SignalChannel, "isExists");
        ok = -1;
    } else if (op == "r" && !isExists) {
        emitFileSignal (SignalChannel, "noExists");
        ok = -1;
    } else if (op == "w" || op == "wx") {
        if (!File.open (QIODevice::WriteOnly | QIODevice::Truncate)) {
            emitFileSignal (SignalChannel, "errorOpen");
            ok = -2;
        } else {
            connect (FileSocket, SIGNAL(readyRead()),
                     this, SLOT(writeFile()));
            ok = 1;
            FileSocket->write (QByteArray((char*)&ok, 1));
        }
    } else if (op == "r") {
        if (!File.open (QIODevice::ReadOnly)) {
            emitFileSignal (SignalChannel, "errorOpen");
            ok = -2;
        } else {
            connect (FileSocket, SIGNAL(bytesWritten(qint64)),
                     this, SLOT(transFile(qint64)));
            ok = 1;
            FileSocket->write (QByteArray((char*)&ok, 1));
            transFile (0);
        }
    }

    if (ok <= 0) {
        FileSocket->write (QByteArray((char*)&ok, 1));
        FileSocket->disconnectFromHost ();
    }
}

void fileStream::writeFile()
{
    File.write (FileSocket->readAll ());
}

void fileStream::transFile(qint64)
{
    if (!File.atEnd ()) {
        QByteArray data = File.read (BUFSIZE);
        FileSocket->write (data);
    } else {
        File.close ();
        FileSocket->disconnectFromHost ();
    }
}

void fileStream::closeFile()
{
    File.close ();
    FileSocket->deleteLater ();
}

void fileStream::emitFileSignal(qulonglong, const QString&)
{

}
