#ifndef LOADER
#define LOADER

#include <QtScript>
#include <bgmrobject.h>

using namespace BGMircroRPCServer;

extern "C" {
void initialModule (QScriptEngine* jsEngine, BGMRObjectInterface*);
void unload ();
}

#endif // LOADER

