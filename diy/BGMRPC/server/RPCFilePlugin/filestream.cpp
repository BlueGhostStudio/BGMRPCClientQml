#include "filestream.h"

fileStream::fileStream(BGMRProcedure* proc, QObject *parent) :
    QObject(parent), OwnProc (proc),
    DataStream (OwnProc->switchDirectSocket ()),
    SocketBuffer (OwnProc->socketBuffer ()),
    StreamStatus (NOACTIVE)
{
//    connect (DataStream, SIGNAL(readyRead()),
//             SLOT(receiveData()), Qt::DirectConnection);
    connect (SocketBuffer, SIGNAL(readyRead()),
             SLOT(receiveData()), Qt::DirectConnection);
    connect (DataStream, SIGNAL(disconnected()),
             SLOT(deleteLater()), Qt::DirectConnection);
    connect (DataStream, SIGNAL(bytesWritten(qint64)),
             SLOT(writenData(qint64)), Qt::DirectConnection);
}

void fileStream::receiveFile(const QString& fileName)
{
    resetStatus ();
    File.setFileName (fileName);
    QJsonArray rets;
    if (!File.open (QIODevice::WriteOnly))
        rets.append (false);
    else {
        StreamOp = RECEIVEFILE;
        StreamStatus = WAITTINGCLIENTREADY;
        rets.append (true);
    }

    OwnProc->returnValues (rets, true);
}

void fileStream::sendFile(const QString& fileName)
{
    resetStatus ();
    File.setFileName (fileName);
    QJsonArray rets;
    if (!File.open (QIODevice::ReadOnly)) {
        qDebug () << "no open";
        rets.append (false);
        OwnProc->returnValues (rets, true);
    } else {
        qDebug () << "opened";
        StreamOp = SENDFILE;
        StreamStatus = READY;
        DataSize = File.size ();
        if (DataSize <= 1024)
            MaxLen = DataSize;
        else
            MaxLen = DataSize / 100;

        rets.append (true);
        rets.append ((double)DataSize);
        OwnProc->returnValues (rets, true);
    }
}

void fileStream::resetStatus()
{
    StreamedSize = 0;
    StreamStatus = NOACTIVE;
    StreamOp = NOOP;
    MaxLen = 0;
    if (File.isOpen ())
        File.close ();
}

void fileStream::receiveData()
{
    qDebug () << "receiveData";
    QByteArray data = SocketBuffer->readAll ();

    if (StreamOp == RECEIVEFILE) {
        if (StreamStatus == WAITTINGCLIENTREADY) {
            DataSize = data.toULongLong ();
            StreamStatus = READING;
        } else if (StreamStatus == READING) {
            StreamedSize += data.size ();
            qDebug () << StreamedSize;
            File.write (data);
            if (StreamedSize >= DataSize) {
                resetStatus ();
//                OwnProc->switchProcedure ();
//                deleteLater ();
            }
        }
    }
}

void fileStream::writenData(qint64 size)
{
    if (StreamOp == SENDFILE) {
        if (StreamStatus == SENDING)
            StreamedSize += size;
        else
            StreamStatus = SENDING;

        if (File.atEnd ()) {
            resetStatus ();
//            OwnProc->switchProcedure ();
//            deleteLater ();
        } else
#ifdef WEBSOCKET
            DataStream->sendBinaryMessage (File.read (MaxLen));
#else
            DataStream->write (File.read (MaxLen));
#endif
    }
}

