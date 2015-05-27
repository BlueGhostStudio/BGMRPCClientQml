#ifndef LOADER_H
#define LOADER_H
#include <QtScript>
#include <bgmrobject.h>

using namespace BGMircroRPCServer;
extern "C" {
void initialModule (QScriptEngine* jsEngine, BGMRObjectInterface* obj);
void unload ();
}

#endif // LOADER_H
