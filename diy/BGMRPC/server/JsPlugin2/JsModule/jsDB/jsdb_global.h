#ifndef JSDB_GLOBAL_H
#define JSDB_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JSDB_LIBRARY)
#  define JSDBSHARED_EXPORT Q_DECL_EXPORT
#else
#  define JSDBSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // JSDB_GLOBAL_H
