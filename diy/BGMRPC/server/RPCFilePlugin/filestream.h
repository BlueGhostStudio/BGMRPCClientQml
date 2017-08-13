#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <QObject>
#include <bgmrprocedure.h>

using namespace BGMircroRPCServer;

class fileStream : public QObject
{
    Q_OBJECT
public:
    explicit fileStream(BGMRClient* proc, QObject *parent = 0);

    void receiveFile (const QString& fileName);
    void sendFile (const QString& fileName);
    void resetStatus ();
    void close () { resetStatus (); }

signals:

private slots:
    void receiveData ();
    void writenData (qint64 size);

private:
    BGMRClient* OwnProc;
    __socket* DataStream;
    BGMRPCSocketBuffer* SocketBuffer;
    QFile File;

    enum STREAMOP {
        NOOP,
        RECEIVEFILE,
        SENDFILE
    };

    enum STREAMSTATUS {
        NOACTIVE,
        WAITTINGCLIENTREADY,
        READING,
        READY,
        SENDING
    };

    STREAMSTATUS StreamStatus;
    STREAMOP StreamOp;
    qulonglong DataSize;
    qulonglong StreamedSize;
    qulonglong MaxLen;
};

#endif // FILESTREAM_H
