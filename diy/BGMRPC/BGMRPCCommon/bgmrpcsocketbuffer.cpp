#include "bgmrpcsocketbuffer.h"

BGMRPCSocketBuffer::BGMRPCSocketBuffer(__socket* socket, QObject* parent) :
    QObject (parent), Socket (socket)
{
#ifdef WEBSOCKET
    /*connect (Socket, SIGNAL(frameReceived(QByteArray)),
             SLOT(onReceivedData(QByteArray)));
    connect (Socket, SIGNAL(frameReceived(QString)),
             SLOT(onReceivedData(QString)));*/
    connect (Socket, SIGNAL(binaryMessageReceived(QByteArray)),
             SLOT(onReceivedData(QByteArray)));
    connect (Socket, SIGNAL(textMessageReceived(QString)),
             SLOT(onReceivedData(QString)));
#else
    connect (Socket, SIGNAL(readyRead()),
             SIGNAL(readyRead()));
#endif
}

QByteArray BGMRPCSocketBuffer::readAll()
{
#ifdef WEBSOCKET
    QByteArray readedBuffer (Buffer);
    Buffer.clear ();

    return readedBuffer;
#else
    return Socket->readAll ();
#endif
}

#ifdef WEBSOCKET
void BGMRPCSocketBuffer::onReceivedData(const QString& message)
{
    Buffer += message.toUtf8 ();

    emit readyRead ();
}

void BGMRPCSocketBuffer::onReceivedData(const QByteArray& message)
{
    Buffer += message;

    emit readyRead ();
}
#endif
