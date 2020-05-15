#include "bgmrpccommon.h"

// namespace NS_BGMRPC {
QString BGMRPCObjPrefix = "BGMRPC_OBJ_";
QString BGMRPCCtrlSocket = "BGMRPC_CTRL";

void splitReturnData(const QByteArray& data,
                     std::function<void(const QByteArray&)> callback)
{
    if (!callback)
        return;

    int i = 0;
    int lenLen = sizeof(quint64);
    while (i < data.length()) {
        quint64 len = bytes2int<quint64>(data.mid(i, lenLen));
        i += lenLen;
        QByteArray retData = data.mid(i, len);
        i += len;
        callback(retData);
    }
}

//} // namespace NS_BGMRPC
