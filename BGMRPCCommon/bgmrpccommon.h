#ifndef BGMRPCCOMMON_H
#define BGMRPCCOMMON_H

#include <QLocalSocket>
#include <functional>

#include "flags.h"
#include "intbytes.h"

extern QByteArray BGMRPCObjPrefix;
extern QByteArray BGMRPCServerCtrlSocket;
// extern QByteArray BGMRPCObjectCtrlSocket;
extern QByteArray BGMRPCObjectSocket;
extern QString defaultEtcDir;

bool splitLocalSocketFragment(QLocalSocket* socket,
                              std::function<void(const QByteArray&)> callback);

QByteArray getSettings(QLocalSocket& ctrlSocket, NS_BGMRPC::Config cnf);
QByteArray getSettings(QLocalSocket& ctrlSocket, const QByteArray& key);
QByteArray genObjectName(const QByteArray& grp, const QByteArray& app,
                         const QByteArray& name, bool noAppPrefix);

template <typename T>
T
ctrlCommand(QLocalSocket* ctrlSocket, quint8 cmd, const QByteArray& arg,
            std::function<T(const QByteArray&)> callback) {
    QByteArray cmdData(1, (quint8)cmd);
    cmdData.append(arg);
    ctrlSocket->write(cmdData);
    if (!ctrlSocket->waitForBytesWritten())
        throw std::runtime_error("Local socket failed to send data.");
    if (!ctrlSocket->waitForReadyRead())
        throw std::runtime_error("Local socket failed to read data.");

    QByteArray readData = ctrlSocket->readAll();

    return callback(readData);
}

bool checkObject(QLocalSocket* ctrlSocket, const QString& objName);

void initialLogMessage(quint8 mf = 0x1f);

#endif  // BGMRPCCOMMON_H
