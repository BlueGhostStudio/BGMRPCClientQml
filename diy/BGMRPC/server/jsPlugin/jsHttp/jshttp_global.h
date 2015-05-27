#ifndef JSHTTP_GLOBAL_H
#define JSHTTP_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JSHTTP_LIBRARY)
#  define JSHTTPSHARED_EXPORT Q_DECL_EXPORT
#else
#  define JSHTTPSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // JSHTTP_GLOBAL_H
