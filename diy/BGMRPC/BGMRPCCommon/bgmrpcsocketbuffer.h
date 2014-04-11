#ifndef BGMRPCSOCKETBUFFER_H
#define BGMRPCSOCKETBUFFER_H

#include "socket.h"

class BGMRPCSocketBuffer : public QObject
{
    Q_OBJECT
public:
    explicit BGMRPCSocketBuffer(__socket* socket, QObject* parent = NULL);

    QByteArray readAll ();

signals:
    void readyRead ();

public slots:
private slots:
#ifdef WEBSOCKET
    void onReceivedData (const QString& frame);
    void onReceivedData (const QByteArray& frame);
#endif

private:
    __socket*  Socket;
#ifdef WEBSOCKET
    QByteArray Buffer;
#endif
};

#endif // BGMRPCSOCKETBUFFER_H
