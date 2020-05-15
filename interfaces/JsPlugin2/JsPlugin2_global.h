#ifndef JSPLUGIN2_GLOBAL_H
#define JSPLUGIN2_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(JSPLUGIN2_LIBRARY)
#  define JSPLUGIN2_EXPORT Q_DECL_EXPORT
#else
#  define JSPLUGIN2_EXPORT Q_DECL_IMPORT
#endif

#endif // JSPLUGIN2_GLOBAL_H
