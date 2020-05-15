#ifndef BGMRPCCOMMON_H
#define BGMRPCCOMMON_H

#include "flags.h"
#include "intbytes.h"
#include <functional>

//#define BGMRPCObjPrefix QString("BGMRPC_OBJ_")
//#define BGMRPCCtrlSocket QString("BGMRPC_CTRL")

// namespace NS_BGMRPC {
extern QString BGMRPCObjPrefix;
extern QString BGMRPCCtrlSocket;

void splitReturnData(const QByteArray& data,
                     std::function<void(const QByteArray&)> callback);
//} // namespace NS_BGMRPC

#endif // BGMRPCCOMMON_H
