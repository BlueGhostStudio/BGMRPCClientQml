#ifndef JSTIMER_GLOBAL_H
#define JSTIMER_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JSTIMER_LIBRARY)
#  define JSTIMERSHARED_EXPORT Q_DECL_EXPORT
#else
#  define JSTIMERSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // JSTIMER_GLOBAL_H
