#ifndef BGMRPCCOMMON_H
#define BGMRPCCOMMON_H

#include <QLocalSocket>
#include <functional>

#include "flags.h"
#include "intbytes.h"

extern QString BGMRPCObjPrefix;
extern QString BGMRPCCtrlSocket;

bool splitLocalSocketFragment(QLocalSocket* socket,
                              std::function<void(const QByteArray&)> callback);

QByteArray getSettings(QLocalSocket& ctrlSocket, NS_BGMRPC::Config cnf);
QByteArray getSettings(QLocalSocket& ctrlSocket, const QString& key);

void initialLogMessage(quint8 mf = 0x1f);

#endif  // BGMRPCCOMMON_H
