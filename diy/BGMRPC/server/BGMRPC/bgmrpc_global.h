#ifndef BGMRPC_GLOBAL_H
#define BGMRPC_GLOBAL_H

#include <QtCore/qglobal.h>
#include <QtCore>

#if defined(BGMRPC_LIBRARY)
#  define BGMRPCSHARED_EXPORT Q_DECL_EXPORT
#else
#  define BGMRPCSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // BGMRPC_GLOBAL_H
