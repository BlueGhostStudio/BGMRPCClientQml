#include "bgmrobjectstorage.h"
#include "bgmrobject.h"
#include <QLibrary>
#include "bgmrpc.h"

namespace BGMircroRPCServer {

BGMRObjectStorage::BGMRObjectStorage(BGMRPC* r)
    : RPC (r)
{
}

BGMRObjectStorage::~BGMRObjectStorage()
{
}

bool BGMRObjectStorage::installObject(const QString& objName,
                                      BGMRObjectInterface* obj)
{
    bool ok = false;
    if (!ObjectStorage.contains (objName)) {
        obj->setObjectName (objName);
        ObjectStorage [objName] = obj;
        ok = true;
    }

    return ok;
}

BGMRObjectInterface* BGMRObjectStorage::installObject(const QString& objName, const QString& creator)
{
    //bool ok = false;
    if (!ObjectStorage.contains (objName)
            && ObjCreators.contains (creator)) {
        BGMRObjectInterface* obj
                = ObjCreators [creator] ();
        if (obj) {
            obj->setObjectName (objName);
            ObjectStorage [objName] = obj;

            return obj;
            //ok = true;
        }
    }

    return NULL;
}

bool BGMRObjectStorage::installPlugin(const QString& pluginFileName)
{
    bool ok = false;
    QString basePluginDir = BGMRPC::Settings->value ("pluginDir",
                                                    "~/.BGMR/plugin").toString ();
    QString path = QString ("%1/%2")
                     .arg(basePluginDir)
                     .arg (pluginFileName);
    QLibrary plugin (path);
    if (plugin.load ()) {
        objCreator_T creator = (objCreator_T)plugin.resolve ("objCreator");
        QString (*objType)() = (QString (*)())plugin.resolve ("objType");
        if (creator && objType) {
            ObjCreators [objType ()] = creator;

            bool (*initial)(BGMRObjectStorage*, BGMRPC*)
                    = (bool (*)(BGMRObjectStorage*, BGMRPC*))plugin.resolve ("initial");
            if (initial) {
                if (!initial (this, RPC))
                    qCritical () << QObject::tr ("Install plugin initial error.");
            }

            ok = true;
        }
    } else
        qDebug () << plugin.errorString ();

    return ok;
}

bool BGMRObjectStorage::removeObject(const QString& objName)
{
    bool ok = false;
    if (ObjectStorage.contains (objName)) {
        BGMRObjectInterface* obj = ObjectStorage.take (objName);
        delete obj;
        ok = true;
    }

    return ok;
}

BGMRObjectInterface*
BGMRObjectStorage::object (const QString& obj) const
{
    return ObjectStorage [obj];
}

QList<BGMRObjectInterface*> BGMRObjectStorage::objects() const
{
    // QList < QPair < QString, QString > > objList;
    QList < BGMRObjectInterface* > objList;
    objectMap::const_iterator it;

    for (it = ObjectStorage.constBegin ();
         it != ObjectStorage.constEnd ();
         ++it) {
        objList.append (it.value ());
    }

    return objList;
}

QStringList BGMRObjectStorage::types() const
{
    return ObjCreators.keys ();
}

}
