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

bool splitLocalSocketFragment(QLocalSocket* socket,
                              std::function<void(const QByteArray&)> callback);

QByteArray getSettings(QLocalSocket& ctrlSocket, NS_BGMRPC::Config cnf);
QByteArray getSettings(QLocalSocket& ctrlSocket, const QByteArray& key);
QByteArray genObjectName(const QByteArray& grp, const QByteArray& app,
                         const QByteArray& name, bool noAppPrefix);

void initialLogMessage(quint8 mf = 0x1f);

#endif  // BGMRPCCOMMON_H
