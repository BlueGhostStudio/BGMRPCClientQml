#include "doodlestorage.h"
#include <QDir>
#include <QMutexLocker>
#include <QSqlQuery>

using namespace NS_BGMRPCObjectInterface;

DoodleStorage::DoodleStorage(QObject* paretnt) : ObjectInterface(paretnt)
{
    registerMethods();

    m_database = QSqlDatabase::addDatabase("QSQLITE");
    m_database.setDatabaseName("py/Handwritten/HWStorage/handwritten.db");
    m_database.open();
}

QVariant DoodleStorage::storageDoodleStrokes(QPointer<Caller> /*caller*/,
                                             const QVariantList& args)
{
    QMutexLocker locker(&m_mutex);

    m_buffer += args[0].toByteArray();

    if (m_buffer.length() >= 64 * 13) {
        QByteArray data;

        QSqlQuery query(m_database);
        query.exec("SELECT * FROM `StrokeData` WHERE `id`=-1");
        if (query.next())
            data = query.value("data").toByteArray();
        data += m_buffer;

        query.prepare(
            "REPLACE INTO `StrokeData` (`id`, `data`) VALUES (-1, :D)");
        query.bindValue(":D", data);
        query.exec();

        m_buffer.clear();
    }

    return QVariant();
}

bool DoodleStorage::verification(QPointer<Caller> caller, const QString& method,
                                 const QVariantList& /*args*/)
{
    if (method == "storageDoodleStrokes" && caller->ID() != -1)
        return false;

    return true;
}

void DoodleStorage::registerMethods()
{
    m_methods["storageDoodleStrokes"] =
        REG_METHOD(DoodleStorage, storageDoodleStrokes);
}

ObjectInterface* create(int, char**)
{
    return new DoodleStorage;
}
