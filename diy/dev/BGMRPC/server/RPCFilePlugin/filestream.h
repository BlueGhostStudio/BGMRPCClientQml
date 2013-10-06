#ifndef FILESTREAM_H
#define FILESTREAM_H

#include <QObject>
#include "socket.h"
#include <QFile>

#define BUFSIZE 1024

class RPCFileObj;

class fileStream : public QObject {
    Q_OBJECT
public:
    /*explicit fileStream (RPCFileObj* fileObj,
                         __socket* socket,
                         const QString& fileName,
                         const QString& op,
                         qulonglong signalChannel,
                         QObject* parent = 0);*/
    explicit fileStream (RPCFileObj* fileObj,
                         __socket* socket,
                         QObject* parent = 0);

    void openFile (const QString& fileName, const QString& op);

private slots:
    void writeFile ();
    void transFile (qint64);
    void closeFile ();

protected:
   RPCFileObj* FileObj;
   __socket* FileSocket;
   qulonglong  SignalChannel;
   QFile File;
   QByteArray Buffer;

   void emitFileSignal (qulonglong, const QString&);
};



#endif // FILESTREAM_H
