#ifndef BGMRPCCOMMON_H
#define BGMRPCCOMMON_H

#include <QLocalSocket>
#include <functional>

#include "flags.h"
#include "intbytes.h"

//#define BGMRPCObjPrefix QString("BGMRPC_OBJ_")
//#define BGMRPCCtrlSocket QString("BGMRPC_CTRL")

// namespace NS_BGMRPC {
extern QString BGMRPCObjPrefix;
extern QString BGMRPCCtrlSocket;

/*void splitData(const QByteArray& data,
                     std::function<void(const QByteArray&)> callback);*/
bool splitLocalSocketFragment(QLocalSocket* socket,
                              std::function<void(const QByteArray&)> callback);

QByteArray getSettings(QLocalSocket& ctrlSocket, NS_BGMRPC::Config cnf);

void initialLogMessage(quint8 mf = 0x1f);
//} // namespace NS_BGMRPC

#endif  // BGMRPCCOMMON_H
