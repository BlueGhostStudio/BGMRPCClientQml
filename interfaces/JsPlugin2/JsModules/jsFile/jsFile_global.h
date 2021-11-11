#ifndef JSFILE_GLOBAL_H
#define JSFILE_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JSFILE_LIBRARY)
#  define JSFILE_EXPORT Q_DECL_EXPORT
#else
#  define JSFILE_EXPORT Q_DECL_IMPORT
#endif

#endif // JSFILE_GLOBAL_H
