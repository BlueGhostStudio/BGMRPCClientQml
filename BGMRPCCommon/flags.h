#ifndef FLAGS_H
#define FLAGS_H
#include <QtCore>

namespace NS_BGMRPC
{
enum flags {
    CTRL_REGISTER = 0,
    CTRL_CHECKOBJECT = 1,
    DATA_ERROR = 2,
    //    CTRL_LOCALCALL = 2,
    DATA_CLIENTID = 3,
    DATA_LOCALCALL_CLIENTID = 4
};

enum error { ERR_NOOBJ = 0, ERR_NOMETHOD = 1, ERR_ACCESS };
} // namespace NS_BGMRPC

#endif // FLAGS_H
